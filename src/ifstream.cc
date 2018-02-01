#include <cassert>
#include <cerrno>
#include <stdexcept>
#include <bitstream/sstream.h>
#include <bitstream/ifstream.h>


namespace bitstream {
namespace input {
namespace file {



const char *Stream::peak(unsigned long size) {
    if (buffer.data.size < size) {
        buffer.defragment();
        buffer.data.size += file.read(buffer.begin + buffer.data.end(), file.offset + buffer.data.size, buffer.can_read(size));
        if (buffer.data.size < size) {
            throw EndOfStream(file.path + ": end of stream");
        }
    }
    return buffer.begin + buffer.data.offset;
}

Blob &Stream::peak_blob(unsigned long size) {
    blob._offset = file.offset;
    blob._size = size;
    return blob;
}

Blob &Stream::get_blob(unsigned long size) {
    blob._offset = file.offset;
    blob._size = size;
    buffer.data -= std::min(size, buffer.data.size);
    file.offset += size;
    return blob;
}



Stream::fstream::fstream(const std::string &path) : path(path) {
    rdbuf()->pubsetbuf(nullptr, 0); // No internal buffering
    open(path);
    if (!is_open()) {
        throw std::runtime_error(path + ": " + std::strerror(errno));
    }
}

unsigned long Stream::fstream::read(char *data, uint64_t offset, unsigned long size) {
    assert(eof() || tellg() <= offset); // Make sure it never goes backward
    seekg(offset);
    std::ifstream::read(data, size);
    return gcount();
}



namespace {
unsigned long block_size() {
    // TODO: find out real block size
    // Think. It sould be specific per device?
    return 512;
}}

Stream::Buffer::Block::Block() {
    static unsigned long size = block_size();
    this->size = size;
}



Stream::Buffer::Buffer(unsigned long size) {
    // Make sure that size has at least 2 blocks and is multiple of block.size
    size = std::max(size, (size / block.size + (size % block.size ? 1: 0)) * block.size);    // Floor
    size = std::max(size, 2 * block.size);
    begin = new char[size];
    this->size = size;
}

Stream::Buffer::~Buffer() {
    delete [] begin;
}

void Stream::Buffer::defragment() {
    // Move leftover to the begin of buffer
    ::memcpy((void *)begin, begin + data.offset, data.size);
    data.offset = 0;
}

unsigned long Stream::Buffer::can_read(unsigned long size) {
    auto required = size - data.size;
    auto capacity = this->capacity();
    if (capacity < required) {
        throw std::runtime_error(SStream() << "Couldn't ensure size of " << size << " bytes which more than capacity of " << capacity << " bytes allows (" << data.size << " bytes already in buffer). The entire buffer should have had more capacity!!!");
    }
    auto block_aligned = capacity / block.size * block.size;
    return required < block_aligned ? block_aligned : capacity; // align if block is enough
}


}}} // namespace bitstream::input::file

