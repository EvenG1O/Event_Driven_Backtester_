#pragma once
#include "../DataEvent/DataEvent.h"
#include "../types/Candle.h"
#include <fstream>
#include <optional>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

class readDataCsv
{

private:
  std::ifstream file;
  std::vector<Candle> barBatch;
  int amountOfRowsToRead{};
  bool fileOpen = false;
  int currentRow{};
  int barIndex{};

public:
  std::vector<Candle> readBinanceCsv();

  void setAmountofRowsToRead(const int numOfRows = 1);

  bool openFile(const std::string &fileName);

  std::optional<DataEvent> newBar();
};
