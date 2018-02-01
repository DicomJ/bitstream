#include <gtest/gtest.h>
#include <bitstream/ifstream.h>


struct FileStreamFixture: ::testing::Test {

    std::string path;

    FileStreamFixture() {
        path = "/tmp/d22d017a-a820-4836-93c4-496342226d6c.data";
        std::ofstream file(path);
        char data[] = "123";
        file.write(data, sizeof(data) - 1);
    }
};

TEST(FileStream, inexisting_path) {
    ASSERT_THROW({ bitstream::input::file::Stream stream("/tmp/9ca56f86-b565-47e2-bf2a-c9a97c08ae63.data"); }, std::exception);
}

TEST_F(FileStreamFixture, nocapacity) {
    bitstream::input::file::Stream stream(path, 1);
    stream.get_blob(512 * 100);
    ASSERT_THROW({ stream.peak(1); }, std::exception);
}

TEST_F(FileStreamFixture, get_blob_small) {
    bitstream::input::file::Stream stream(path);
    stream.get_blob(3);
    ASSERT_THROW({ stream.peak(1); }, bitstream::input::file::Stream::EndOfStream);
}

TEST_F(FileStreamFixture, overget_blob) {
    bitstream::input::file::Stream stream(path);
    stream.get_blob(100);
    ASSERT_THROW({ stream.peak(1); }, bitstream::input::file::Stream::EndOfStream);
}
