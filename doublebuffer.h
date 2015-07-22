#pragma once
#include <cstdint>
#include <vector>
#include <mutex>
#include <condition_variable>

class DoubleBuffer
{

public:
  DoubleBuffer(uint16_t size=0);
  ~DoubleBuffer();
  bool write(uint16_t pos, int16_t value);
  int16_t read(uint16_t pos);
  int16_t wait_until_full(int16_t called_sequence);

private:
  uint16_t m_size;
  std::vector<int16_t> *m_firstBuffer;
  std::vector<int16_t> *m_secondBuffer;
  bool m_full;
  int16_t m_sequence;

  std::mutex m_lock;
  std::condition_variable m_cvfull;

};
