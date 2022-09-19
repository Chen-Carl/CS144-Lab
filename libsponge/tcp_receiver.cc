#include "tcp_receiver.hh"

// Dummy implementation of a TCP receiver

// For Lab 2, please replace with a real implementation that passes the
// automated checks run by `make check_lab2`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

TCPReceiver::TCPReceiver(const size_t capacity) : 
    _reassembler(capacity), 
    _capacity(capacity),
    _isn(0) {
    
}

void TCPReceiver::segment_received(const TCPSegment &seg) {
    if (_syn == true && seg.header().syn == true) {
        return;
    }
    if (_syn == false && seg.header().syn == false) {
        return;
    }
    if (seg.header().syn == true) {
        _syn = true;
        _isn = seg.header().seqno;
    }
    if (seg.header().fin == true) {
        _fin = true;
    }
    size_t abs_seq = unwrap(seg.header().seqno, _isn, _reassembler.unassembled_pos());
    abs_seq = seg.header().syn ? 0 : abs_seq - 1;
    _reassembler.push_substring(seg.payload().copy(), abs_seq, seg.header().fin);
}

optional<WrappingInt32> TCPReceiver::ackno() const {
    if (_syn == false) {
        return std::nullopt;
    }
    if (_fin && _reassembler.unassembled_bytes() == 0) {
        return wrap(_reassembler.stream_out().bytes_written() + 1, _isn) + 1;
    }
    return wrap(_reassembler.stream_out().bytes_written() + 1, _isn);
}

size_t TCPReceiver::window_size() const { 
    return _capacity - stream_out().buffer_size();
}
