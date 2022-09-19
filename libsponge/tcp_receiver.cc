#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

TCPReceiver::TCPReceiver(const size_t capacity) : 
    m_reassembler(capacity), 
    m_capacity(capacity),
    m_isn(0) {
    
}

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if (m_syn == true && seg.header().syn == true) {
        return;
    }
    if (m_syn == false && seg.header().syn == false) {
        return;
    }
    if (seg.header().syn == true) {
        m_syn = true;
        m_isn = seg.header().seqno;
    }
    if (seg.header().fin == true) {
        m_fin = true;
    }
    size_t abs_seq = unwrap(seg.header().seqno, m_isn, m_reassembler.unassembled_pos());
    abs_seq = seg.header().syn ? 0 : abs_seq - 1;
    m_reassembler.push_substring(seg.payload().copy(), abs_seq, seg.header().fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (m_syn == false) {
        return std::nullopt;
    }
    if (m_fin && m_reassembler.unassembled_bytes() == 0) {
        return wrap(m_reassembler.stream_out().bytes_written() + 1, m_isn) + 1;
    }
    return wrap(m_reassembler.stream_out().bytes_written() + 1, m_isn);
}

size_t TCPReceiver::window_size() const { 
    return m_capacity - stream_out().buffer_size();
}
