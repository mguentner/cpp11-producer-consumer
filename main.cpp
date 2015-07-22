#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <thread>

#include "doublebuffer.h"

#define BUFFER_SIZE 20
#define CONSUMERS 4
#define PRODUCERS 1
#define ROUNDS 3

DoubleBuffer buffer(BUFFER_SIZE);

void produce() {
  std::cout << "Producer:" << "Starting to produce" << std::endl;
  for(uint16_t i=0; i<ROUNDS; i++) {
    std::cout << "Producer:" << "Round #" << i << std::endl;
    for(uint16_t j=0; j<BUFFER_SIZE;j++) {
      std::default_random_engine generator;
      std::uniform_int_distribution<uint16_t> distribution(42, 1337);
      buffer.write(j, distribution(generator));
      /* Simulate data recording */
      std::this_thread::sleep_for (std::chrono::milliseconds(20));
    }
  }
  std::cout << "Producer:" << "Finished with producing" << std::endl;
}

void consume(int id) {
  uint16_t round = 0;
  int16_t sequence = -1;
  std::cout << "Consumer" << id << ": Starting to consume" << std::endl;
  while(round < ROUNDS) {
    std::cout << "Consumer" << id << ": Waiting for data to be ready." << std::endl;
    /* Wait until the producer has produced a full buffer */
    sequence = buffer.wait_until_full(sequence);
    std::cout << "Consumer" << id << ": Data is ready!" << std::endl;
    round++;
  }
  std::cout << "Consumer" << id << ": Finished with consuming" << std::endl;
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
