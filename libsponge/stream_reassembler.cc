#include "stream_reassembler.hh"

// Dummy implementation of a stream reassembler.

// For Lab 1, please replace with a real implementation that passes the
// automated checks run by `make check_lab1`.

// You will need to add private members to the class declaration in `stream_reassembler.hh`

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

StreamReassembler::StreamReassembler(const size_t capacity) : 
    m_output(capacity), 
    m_capacity(capacity), 
    m_unassembled(std::map<uint64_t, std::string>()),
    m_eof(std::numeric_limits<uint64_t>::max()) {

}

//! \details This function accepts a substring (aka a segment) of bytes,
//! possibly out-of-order, from the logical stream, and assembles any newly
//! contiguous substrings and writes them into the output stream in order.
void StreamReassembler::push_substring(const string &data, const size_t index, const bool eof) {
    if (eof) {
        m_eof = index + data.size();
    }
    // add to unassembled map
    size_t maxPos = m_pos + m_capacity - m_output.buffer_size();
    if (m_pos < maxPos) {
        size_t len = data.size();
        if (index + len > maxPos) {
            len = maxPos - index;
        }
        if (m_unassembled.count(index)) {
            if (m_unassembled[index].size() < len) {
                m_unassembled[index] = data.substr(0, len);
            }
        } else {
            m_unassembled[index] = data.substr(0, len);
        }
    }
    // check the output stream
    auto it = m_unassembled.begin();
    // if it->first <= m_pos, we need to write it to the output stream
    while (it != m_unassembled.end() && it->first <= m_pos) {
        if (m_pos - it->first <= it->second.size()) {
            std::string append = it->second.substr(m_pos - it->first);
            m_output.write(append);
            m_pos += append.size();
            if (m_pos == m_eof) {
                m_output.end_input();
            }
        }
        m_unassembled.erase(it);
        it = m_unassembled.begin();
    }
}

size_t StreamReassembler::unassembled_bytes() const { 
    size_t unassembledBytes = 0;
    size_t right = 0;
    for (auto it = m_unassembled.begin(); it != m_unassembled.end(); it++) {
        size_t begin = it->first;
        size_t end = begin + it->second.size();
        if (begin > right) {
            unassembledBytes += end - begin;
            right = end;
        } else if (end > right) {
            unassembledBytes += end - right;
            right = end;
        }
    }
    return unassembledBytes;
}

bool StreamReassembler::empty() const { 
    return m_unassembled.empty(); 
}
