#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>
#include <mutex>

#include "doublebuffer.h"

#define BUFFER_SIZE 20
#define CONSUMERS 4
#define PRODUCERS 1
#define ROUNDS 5
#define REC_DELAY 20

#define DELAY_TEST 0

DoubleBuffer buffer(BUFFER_SIZE);

std::mutex coutlck;
#define LOCK_STDOUT   coutlck.lock()
#define UNLOCK_STDOUT coutlck.unlock()

void produce() {
  LOCK_STDOUT;
  std::cout << "Producer:" << "Starting to produce" << std::endl;
  UNLOCK_STDOUT;
  for(uint16_t i=0; i<ROUNDS; i++) {
    LOCK_STDOUT;
    std::cout << "Producer:" << "Round #" << i << std::endl;
    UNLOCK_STDOUT;
    for(uint16_t j=0; j<BUFFER_SIZE;j++) {
      std::default_random_engine generator;
      std::uniform_int_distribution<uint16_t> distribution(42, 1337);
      buffer.write(j, distribution(generator));
      /* Simulate data recording */
      std::this_thread::sleep_for (std::chrono::milliseconds(REC_DELAY));
    }
  }
  LOCK_STDOUT;
  std::cout << "Producer:" << "Finished with producing" << std::endl;
  UNLOCK_STDOUT;
}

void consume(int id) {
  uint16_t round = 0;
  int16_t sequence = -1;
  int16_t old_sequence = -1;
  LOCK_STDOUT;
  std::cout << "Consumer" << id << ": Starting to consume" << std::endl;
  UNLOCK_STDOUT;
  while(round < ROUNDS) {
    LOCK_STDOUT;
    std::cout << "Consumer" << id << ": Waiting for data to be ready." << std::endl;
    UNLOCK_STDOUT;
#if DELAY_TEST
    if (round == 0 && id == 1) {
      std::cout << "Consumer" << id << ": Delaying...." << std::endl;
      std::this_thread::sleep_for (std::chrono::milliseconds(3*REC_DELAY*BUFFER_SIZE));
    }
#endif
    /* Wait until the producer has produced a full buffer */
    old_sequence = sequence;
    sequence = buffer.wait_until_full(sequence);
    LOCK_STDOUT;
    std::cout << "Consumer" << id << ": Data is ready. sequence changed:" << old_sequence << "->" << sequence << std::endl;
    UNLOCK_STDOUT;
    round++;
  }
  LOCK_STDOUT;
  std::cout << "Consumer" << id << ": Finished with consuming" << std::endl;
  UNLOCK_STDOUT;
}

int main(void) {
  std::thread threads[CONSUMERS+PRODUCERS];
  for(uint16_t i=0; i<CONSUMERS; i++) {
    threads[i] = std::thread(consume, i);
  }
  /* Wait for all consumers */
  std::this_thread::sleep_for (std::chrono::milliseconds(500));
  threads[CONSUMERS+PRODUCERS-1] = std::thread(produce);
  /* Join all threads */
  for(auto& th : threads) th.join();

  return 0;
}
