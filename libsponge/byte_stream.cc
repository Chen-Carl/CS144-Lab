#include "byte_stream.hh"

// Dummy implementation of a flow-controlled in-memory byte stream.

// For Lab 0, please replace with a real implementation that passes the
// automated checks run by `make check_lab0`.

// You will need to add private members to the class declaration in `byte_stream.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

ByteStream::ByteStream(const size_t capacity) :
    m_byteArray(std::deque<char>()) {
    m_capacity = capacity;
}

size_t ByteStream::write(const string &data) {
    // write only if _byteArray.size() < _capacity
    size_t i = 0;
    while (!m_endInput && m_byteArray.size() < m_capacity && i < data.size()) {
        m_byteArray.push_back(data[i]);
        i++;
    }
    m_writtenBytes += i;
    return i;
}

//! \param[in] len bytes will be copied from the output side of the buffer
string ByteStream::peek_output(const size_t len) const {
    string res;
    size_t i = 0;
    while (i < len && i < m_byteArray.size()) {
        res += m_byteArray[i];
        i++;
    }
    return res;
}

//! \param[in] len bytes will be removed from the output side of the buffer
void ByteStream::pop_output(const size_t len) { 
    size_t i = 0;
    while (i < len && !m_byteArray.empty()) {
        m_byteArray.pop_front();
        i++;
    }
    m_readBytes += i;
}

//! Read (i.e., copy and then pop) the next "len" bytes of the stream
//! \param[in] len bytes will be popped and returned
//! \returns a string
std::string ByteStream::read(const size_t len) {
    std::string res = peek_output(len);
    pop_output(len);
    return res;
}

void ByteStream::end_input() {
    m_endInput = true;
}

bool ByteStream::input_ended() const { 
    return m_endInput; 
}

size_t ByteStream::buffer_size() const { 
    return m_byteArray.size(); 
}

bool ByteStream::buffer_empty() const { 
    return m_byteArray.empty(); 
}

bool ByteStream::eof() const { 
    return m_endInput && buffer_empty(); 
}

size_t ByteStream::bytes_written() const { 
    return m_writtenBytes; 
}

size_t ByteStream::bytes_read() const { 
    return m_readBytes; 
}

size_t ByteStream::remaining_capacity() const {
    return m_capacity - m_byteArray.size();; 
}
