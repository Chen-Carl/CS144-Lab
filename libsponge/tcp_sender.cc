#include "tcp_sender.hh"

#include "tcp_config.hh"

#include <algorithm>
#include <random>

// Dummy implementation of a TCP sender

// For Lab 3, please replace with a real implementation that passes the
// automated checks run by `make check_lab3`.

template <typename... Targs>
void DUMMY_CODE(Targs &&... /* unused */) {}

using namespace std;

//! \param[in] capacity the capacity of the outgoing byte stream
//! \param[in] retx_timeout the initial amount of time to wait before retransmitting the oldest outstanding segment
//! \param[in] fixed_isn the Initial Sequence Number to use, if set (otherwise uses a random ISN)
TCPSender::TCPSender(const size_t capacity, const uint16_t retx_timeout, const std::optional<WrappingInt32> fixed_isn)
    : m_isn(fixed_isn.value_or(WrappingInt32{random_device()()}))
    , m_initial_retransmission_timeout{retx_timeout}
    , m_retransmission_timeout{retx_timeout}
    , m_stream(capacity) {}

uint64_t TCPSender::bytes_in_flight() const {
    return m_next_seqno - m_acked_seqno;
}

void TCPSender::fill_window() {
    TCPSegment seg;
    seg.header().seqno = wrap(m_next_seqno, m_isn);
    if (m_state == TCPState::CLOSED) {
        seg.header().syn = true;
        m_segments_out.push(seg);
        m_next_seqno = 1;
        m_state = TCPState::SYN_SENT;
        m_segments_in_flight[m_next_seqno] = seg;
    }
    while (!m_stream.buffer_empty()) {
        if (bytes_in_flight() >= m_window_size) {
            break;
        }
        size_t len = min(m_stream.buffer_size(), m_window_size - bytes_in_flight());
        seg.payload() = Buffer(m_stream.read(len));
        m_segments_out.push(seg);
        m_next_seqno += len;
        m_segments_in_flight[m_next_seqno] = seg;
    }
    if (m_state == TCPState::SYN_ACKED && m_stream.eof() && m_segments_in_flight.empty()) {
        seg.header().fin = true;
        m_segments_out.push(seg);
        m_next_seqno += 1;
        m_state = TCPState::FIN_SENT;
        m_segments_in_flight[m_next_seqno] = seg;
    }
    if (!m_tick_started) {
        m_tick_started = true;
    }
}

//! \param ackno The remote receiver's ackno (acknowledgment number)
//! \param window_size The remote receiver's advertised window size
void TCPSender::ack_received(const WrappingInt32 ackno, const uint16_t window_size) {
    uint64_t acked_seqno = unwrap(ackno, m_isn, m_next_seqno);
    if (acked_seqno > m_next_seqno) {
        return;
    }
    m_acked_seqno = acked_seqno;
    if (m_state == TCPState::SYN_SENT) {
        m_state = TCPState::SYN_ACKED;
    }
    m_window_size = window_size;
    while (!m_segments_in_flight.empty() && m_segments_in_flight.begin()->first <= m_acked_seqno) {
        m_segments_in_flight.erase(m_segments_in_flight.begin());
    }
    m_retransmission_time = 0;
    m_retransmission_timeout = m_initial_retransmission_timeout;
    m_ticks = 0;
}

//! \param[in] ms_since_last_tick the number of milliseconds since the last call to this method
void TCPSender::tick(const size_t ms_since_last_tick) {
    if (m_tick_started) {
        m_ticks += ms_since_last_tick;
    }
    if (m_ticks >= m_retransmission_timeout) {
        m_segments_out.push(m_segments_in_flight.begin()->second);
        m_retransmission_timeout *= 2;
        m_retransmission_time++;
        m_ticks = 0;
    }
}

unsigned int TCPSender::consecutive_retransmissions() const { 
    return m_retransmission_time; 
}

void TCPSender::send_empty_segment() {
    TCPSegment seg;
    seg.header().seqno = wrap(m_next_seqno, m_isn);
    m_segments_out.push(seg);
}
