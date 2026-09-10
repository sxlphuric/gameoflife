#include <iostream>
#include <cstdint>
#include <chrono>
#include <thread>
#include <cmath>
#include <cstdlib>

using namespace std::chrono_literals;
using namespace std::this_thread;
using byte = uint8_t;

const int CHUNK_SIZE = 8;
const int CAMERA_SIZE = 8;
const int MAX_PAN_SPEED = 1;
const int MAX_CHUNKS = 512;
const uint16_t MAX_GENERATIONS = 300000;

bool game = false;
bool autoplay = true;

int chunksCount = 0;

struct Chunk {
    int x;
    int y;
    byte cells[CHUNK_SIZE];
};

Chunk chunks[MAX_CHUNKS] = {};
Chunk nextChunks[MAX_CHUNKS] = {};

byte camera[8];

int cameraX = 0;
int cameraY = 0;

int world = 1;
bool speedMeasured = false;

int floorDiv8(int n) {
  if (n >= 0) return n / 8;
  return (n - 7) / 8;
}

int mod8(int n) {
  int r = n & 7;
  if (r < 0) r += 8;
  return r;
}

bool bitRead(byte byte, int b) {
    uint8_t mask = 1 << b;
    return ((byte & mask) >> b) == 0b1;
}

void bitWrite(byte& byte, int b, bool value) {
    uint8_t mask = 1 << b;
    if (value) {
        byte |= mask;
    } else {
        byte &= ~mask;
    }
}



bool getCell(int x, int y) {
  int chunkX = floorDiv8(x);
  int chunkY = floorDiv8(y);
  int xInChunk = mod8(x);
  int yInChunk = mod8(y);
  for (int i = 0; i < chunksCount; i++) {
    if (chunks[i].x == chunkX && chunks[i].y == chunkY) {
      return bitRead(chunks[i].cells[yInChunk], xInChunk);
    }
  }
  return false;
}


int setCell(int x, int y, bool value) {
  int chunkX = floorDiv8(x);
  int chunkY = floorDiv8(y);
  int xInChunk = mod8(x);
  int yInChunk = mod8(y);
  for (int i = 0; i < chunksCount; i++) {
    if (chunks[i].x == chunkX && chunks[i].y == chunkY) {
      bitWrite(chunks[i].cells[yInChunk], xInChunk, value);
      return 0;
    };
  };

  if (chunksCount >= MAX_CHUNKS) {
    return -1;
  }

  Chunk newChunk = {};

  newChunk.x = chunkX;
  newChunk.y = chunkY;
  bitWrite(newChunk.cells[yInChunk], xInChunk, value);
  chunks[chunksCount] = newChunk;
  chunksCount += 1;
  return 0;
}



void clearCameraRow(int row) {
  camera[row] = 0;
}

void clearCamera() {
  for (uint8_t row = 0; row < 8; row++) {
    clearCameraRow(row);
  }
}

byte getChunkByte(int x, int y, int byte) {
  for (int i = 0; i < chunksCount; i++) {
    if (chunks[i].x == x && chunks[i].y == y) {
      return chunks[i].cells[byte];
    }
  }
  return 0;
}

void gliderGun() {
  cameraX = 0;
  cameraY = 0;

  // eater
  setCell(1,-3, true);
  setCell(0,-3,true);
  setCell(1,-4,true);
  setCell(0,-5,true);
  setCell(-1,-5,true);
  setCell(-2,-5,true);
  setCell(-2,-6,true);

  // the gun
  setCell(0, 15, true);
  setCell(0, 16, true);
  setCell(1, 15, true);
  setCell(1, 16, true);

  setCell(11, 12, true);
  setCell(11, 13, true);
  setCell(11, 17, true);
  setCell(11, 18, true);

  setCell(13, 13, true);
  setCell(13, 17, true);
  setCell(14, 14, true);
  setCell(14, 15, true);
  setCell(14, 16, true);
  setCell(15, 14, true);
  setCell(15, 15, true);
  setCell(15, 16, true);

  setCell(18, 13, true);
  setCell(19, 12, true);
  setCell(19, 13, true);
  setCell(19, 14, true);
  setCell(20, 11, true);
  setCell(20, 15, true);

  setCell(21, 13, true);

  setCell(22, 10, true);
  setCell(23, 10, true);
  setCell(22, 16, true);
  setCell(23, 16, true);
  setCell(24, 11, true);
  setCell(24, 15, true);
  setCell(25, 12, true);
  setCell(25, 13, true);
  setCell(25, 14, true);

  setCell(34, 13, true);
  setCell(34, 14, true);
  setCell(35, 13, true);
  setCell(35, 14, true);
}
void rPentomino() {
  cameraX = 0;
  cameraY = 0;

  setCell(0, 1, true);
  setCell(0, 0, true);
  setCell(0, -1, true);
  setCell(1, 0, true);
  setCell(-1, 1, true);
}
void randomMap() {
  cameraX = 0;
  cameraY = 0;
  for (int8_t r = -8; r < 8; r++) {
    for (int8_t c = -8; c < 8; c++) {
      if (std::rand() % 1 <= 30) {
        setCell(r, c, true);
      }
    }
  }
}

void clearBoard() {
    chunksCount = 0;
}

void handleMapPreset(int PRESET) {
  switch (PRESET) {
    case 1:
      gliderGun();
      break;
    case 2:
      rPentomino();
      break;
    case 3:
      randomMap();
      break;
    default:
      gliderGun();
      break;
  };
};

void resetBoard() {
  clearBoard();
  handleMapPreset(world);
  clearCamera();
};

void updateCamera() {
  for (uint8_t row = 0; row < CAMERA_SIZE; row++) {
    camera[row] = getChunkByte(cameraX, cameraY, row);
  }
  std::cout << "camera x: " << cameraX << std::endl;
  std::cout << "camera y: " << cameraY << std::endl;
}

void displayByte(byte by) {
    for (uint8_t b = 0; b < 8; b++) {
        std::cout << (bitRead(by,b) ? '#' : '.');
    }
}

void display() {
  for (uint8_t row = 0; row < 8; row++) {
    displayByte(camera[row]);
    std::cout << std::endl;
  };
}









bool getCameraCell(int row, int col) {
  return bitRead(camera[row], col);
}

void setCameraCell(int row, int col, bool value) {
  bitWrite(camera[row], col, value);
}

void fillCameraRow(int row) {
  camera[row] = 0xFF;
}




void transition(long speed, long pause) {
  for (uint8_t row = 0; row < 8; row++) {
    fillCameraRow(row);
    display();
    sleep_for(std::chrono::milliseconds(speed));
  };

  // digitalWrite(BUZZER, LOW);

  sleep_for(std::chrono::milliseconds(pause));

  for (uint8_t row = 0; row < 8; row++) {
    clearCameraRow(row);
    display();
    sleep_for(std::chrono::milliseconds(speed));
  };
}

void addByte(byte t, byte b, byte l, byte r, byte tl, byte tr, byte bl, byte br, uint8_t bitCount[8]) {
  for (uint8_t bit = 0; bit < 8; bit++) {
    if (bitRead(t, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(b, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(l, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(r, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(tl, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(tr, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(bl, bit)) {
      bitCount[bit] += 1;
    };
    if (bitRead(br, bit)) {
      bitCount[bit] += 1;
    };
  }
}

void nextGeneration() {
  int newChunkCount = 0;
  for (int chunk = 0; chunk < chunksCount; chunk++) {
    int chunkX = chunks[chunk].x;
    int chunkY = chunks[chunk].y;

    for (int x = chunkX - 1; x <= chunkX + 1; x++) {
      for (int y = chunkY - 1; y <= chunkY + 1; y++) {
        bool exists = false;
        for (int newChunk = 0; newChunk < newChunkCount; newChunk++) {
          if (nextChunks[newChunk].x == x && nextChunks[newChunk].y == y) {
            exists = true;
            break;
          }
        }

        if (!exists) {

          nextChunks[newChunkCount].x = x;
          nextChunks[newChunkCount].y = y;

          for (uint8_t row = 0; row < 8; row++) {
            nextChunks[newChunkCount].cells[row] = 0;
          }

          newChunkCount++;
        }
      }
    }
  }

  for (int chunk = 0; chunk < newChunkCount; chunk++) {
    int x = nextChunks[chunk].x;
    int y = nextChunks[chunk].y;
    for (uint8_t byteindex = 0; byteindex < 8; byteindex++) {
      byte top = getChunkByte(x, y - 1, 7);
      byte mid = getChunkByte(x, y, byteindex);
      byte bot = getChunkByte(x, y + 1, 0);

      if (byteindex > 0) {
        top = getChunkByte(x, y, byteindex - 1);
      }

      if (byteindex < 7) {
        bot = getChunkByte(x, y, byteindex + 1);
      }

      byte left = mid << 1 | getChunkByte(x - 1, y, byteindex) >> 7;
      byte right = mid >> 1 | getChunkByte(x + 1, y, byteindex) << 7;

      byte topleft = top << 1;
      byte topright = top >> 1;

      byte botleft = bot << 1;
      byte botright = bot >> 1;

      if (byteindex > 0) {
        topleft |= getChunkByte(x - 1, y, byteindex - 1) >> 7;
        topright |= getChunkByte(x + 1, y, byteindex - 1) << 7;
      } else {
        topleft |= getChunkByte(x - 1, y - 1, 7) >> 7;
        topright |= getChunkByte(x + 1, y - 1, 7) << 7;
      }

      if (byteindex < 7) {
        botleft |= getChunkByte(x - 1, y, byteindex + 1) >> 7;
        botright |= getChunkByte(x + 1, y, byteindex + 1) << 7;
      } else {
        botleft |= getChunkByte(x - 1, y + 1, 0) >> 7;
        botright |= getChunkByte(x + 1, y + 1, 0) << 7;
      }

      uint8_t neighbors[8] = { 0 };
      addByte(top, bot, left, right, topleft, topright, botleft, botright, neighbors);

      for (uint8_t nCount = 0; nCount < 8; nCount++) {
        bool alive = bitRead(mid, nCount);

        bitWrite(nextChunks[chunk].cells[byteindex], nCount, ((alive && neighbors[nCount] == 2) || neighbors[nCount] == 3));
      }
    };
  };

  int finalCount = 0;

  for (int c = 0; c < newChunkCount; c++) {
    for (uint8_t row = 0; row < 8; row++) {
      if (nextChunks[c].cells[row] != 0) {
        chunks[finalCount] = nextChunks[c];
        finalCount++;
        break;
      }
    }
  }

  std::cout << "old chunks: ";
  std::cout << chunksCount;

  std::cout << " candidates: ";
  std::cout << newChunkCount;

  std::cout << " final: ";
  std::cout << finalCount << std::endl;

            if (finalCount >= MAX_CHUNKS) {
            std::cout << "optimize yo shit\n";
            return;
          }

  chunksCount = finalCount;
}

void moveCamera() {
  // cameraX += mappedStickX;
  // cameraY += mappedStickY;

  sleep_for(250ms);
  if (!autoplay) sleep_for(25ms);
}



void nextFrame() {
    using namespace std::chrono;
  auto start = system_clock::now();
  nextGeneration();

  updateCamera();
  auto end = system_clock::now();
  if (!speedMeasured) {
    speedMeasured = true;
    std::cout << "Generation compute speed: " << end - start << "ms\n";
  };
}



void setup() {
    std::srand(std::time(0));
    updateCamera();
    display();
}

void loop() {
    if (game) {
      moveCamera();

      if (autoplay) {
        nextFrame();
      } else {
        updateCamera();
      }
      /*} else if (digitalRead(STICK.SW) == LOW) {
      handleMapPreset(world);
      game = true;
      digitalWrite(BUZZER, HIGH);
      transition(50, 90);*/
    } else {
      clearCamera();
      switch (world) {
        case 1:
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(2, 2, true);
          setCameraCell(2, 4, true);
          setCameraCell(3, 4, true);
          setCameraCell(4, 4, true);
          setCameraCell(5, 4, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 2, true);
          setCameraCell(6, 3, true);
          setCameraCell(6, 5, true);
          break;
        case 2:
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(2, 2, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 5, true);
          setCameraCell(4, 4, true);
          setCameraCell(4, 3, true);
          setCameraCell(5, 2, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 2, true);
          setCameraCell(6, 3, true);
          setCameraCell(6, 5, true);
          break;
        case 3:
          setCameraCell(1, 2, true);
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(1, 5, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 4, true);
          setCameraCell(4, 3, true);
          setCameraCell(4, 4, true);
          setCameraCell(4, 5, true);
          setCameraCell(5, 5, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 2, true);
          setCameraCell(6, 3, true);
          break;
        case 4:
          setCameraCell(1, 2, true);
          setCameraCell(1, 5, true);
          setCameraCell(2, 2, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 2, true);
          setCameraCell(3, 5, true);
          setCameraCell(4, 2, true);
          setCameraCell(4, 3, true);
          setCameraCell(4, 4, true);
          setCameraCell(4, 5, true);
          setCameraCell(5, 5, true);
          setCameraCell(6, 5, true);
          break;
        case 5:
          setCameraCell(1, 2, true);
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(1, 5, true);
          setCameraCell(2, 2, true);
          setCameraCell(3, 2, true);
          setCameraCell(3, 3, true);
          setCameraCell(3, 4, true);
          setCameraCell(4, 5, true);
          setCameraCell(5, 2, true);
          setCameraCell(5, 5, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 3, true);
          break;
        case 6:
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(2, 2, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 2, true);
          setCameraCell(4, 2, true);
          setCameraCell(4, 3, true);
          setCameraCell(4, 4, true);
          setCameraCell(5, 2, true);
          setCameraCell(5, 5, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 3, true);
          break;
        case 7:
          setCameraCell(1, 2, true);
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(1, 5, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 4, true);
          setCameraCell(4, 3, true);
          setCameraCell(5, 3, true);
          setCameraCell(6, 3, true);
          break;
        case 8:
          setCameraCell(1, 2, true);
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(1, 5, true);
          setCameraCell(2, 2, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 3, true);
          setCameraCell(3, 4, true);
          setCameraCell(4, 2, true);
          setCameraCell(4, 5, true);
          setCameraCell(5, 2, true);
          setCameraCell(5, 5, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 2, true);
          setCameraCell(6, 3, true);
          setCameraCell(6, 5, true);
          break;
        case 9:
          setCameraCell(1, 2, true);
          setCameraCell(1, 3, true);
          setCameraCell(1, 4, true);
          setCameraCell(1, 5, true);
          setCameraCell(2, 2, true);
          setCameraCell(2, 5, true);
          setCameraCell(3, 2, true);
          setCameraCell(3, 3, true);
          setCameraCell(3, 4, true);
          setCameraCell(3, 5, true);
          setCameraCell(4, 5, true);
          setCameraCell(5, 5, true);
          setCameraCell(6, 4, true);
          setCameraCell(6, 2, true);
          setCameraCell(6, 3, true);
          setCameraCell(6, 5, true);
          break;
      }
    }

    display();
}

int main() {
    game = true;
    setup();
    handleMapPreset(world);
    for(int i = 0; i < MAX_GENERATIONS; i++) {
        loop();
    }
    return 0;
}
