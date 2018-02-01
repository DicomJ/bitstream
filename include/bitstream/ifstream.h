#ifndef __BITSTREAM_IFSTREAM_H__
#define __BITSTREAM_IFSTREAM_H__

#include <cstring>
#include <fstream>
#include <bitstream/stream.h>
#include <bitstream/blob.h>


namespace bitstream {
namespace input {
namespace file {


struct Stream: bitstream::Stream {

    Stream(const std::string &path, unsigned long capacity = 4 * 512)
        : file(path), buffer(capacity) {}

    virtual uint64_t offset() const { return file.offset; }

    virtual const char *peak(unsigned long size);
    virtual Blob &peak_blob(unsigned long size);
    virtual Blob &get_blob(unsigned long size);

private:
    struct fstream: std::ifstream {

        fstream(const std::string &path);
        unsigned long read(char *data, uint64_t offset, unsigned long size);

        std::string path;           // Path of the file from which data is read
        uint64_t offset = 0;        // Absolute offset in the stream from wich peak/read returns data
    } file;

    struct Buffer {

        void defragment();
        unsigned long can_read(unsigned long size);
        unsigned long capacity() { return size - data.size; }

        Buffer(unsigned long size);
        ~Buffer();

        struct Block {
            Block();
            unsigned long size;
        } block;

        struct {
            void operator -= (unsigned long size) {
                this->offset += size;
                this->size -= size;
            }

            unsigned long end() const { return offset + size; }

            unsigned long offset = 0,   // offset inside the buffer from were next read happens
                            size = 0;   // size of data left to read
        } data;

        char *begin;        // Begin of ther buffer
        unsigned long size; // Size of the entire buffer

    } buffer;

    struct Blob_: bitstream::Blob {
        uint64_t _offset = 0;        // Absolute offset in the stream
        unsigned long _size;

        virtual unsigned long size() const {
            return _size;
        }
    } blob;
};

}}} // namespace bitstream::input::file


#endif // __BITSTREAM_IFSTREAM_H__

