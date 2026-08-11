#include "ReadDataCsv.h"

std::vector<Candle> readDataCsv::readBinanceCsv()
{
  std::vector<Candle> candles;

  int current = currentRow;

  if (!file.is_open())
  {
    std::cerr << "file is not open" << std::endl;
    return candles;
  }

  std::string line;

  while (current < currentRow + amountOfRowsToRead &&
         std::getline(file, line))
  {
    if (line.empty())
      continue;
    std::stringstream ss(line);
    std::string token;
    std::vector<std::string> fields;

    while (std::getline(ss, token, ','))
    {
      fields.push_back(token);
    }
    if (fields.size() < 12)
      continue;

    Candle c;
    c.date_open = fields[0];
    c.open = stod(fields[1]);
    c.high = stod(fields[2]);
    c.low = stod(fields[3]);
    c.close = stod(fields[4]);
    c.volume = stod(fields[5]);
    c.close_date = fields[6];
    candles.push_back(c);

    current++;
  }

  currentRow = current;

  return candles;
}

void readDataCsv::setAmountofRowsToRead(const int numOfRows)
{
  amountOfRowsToRead = numOfRows;
}

bool readDataCsv::openFile(const std::string &filename)
{

  if (fileOpen && file.is_open())
  {
    std::cerr << "file already open";
    return false;
  }

  file.open(filename);
  if (!file.is_open())
  {
    std::cerr << "could not open file: " << filename << std::endl;
    return false;
  }

  fileOpen = true;

  // get the header line
  std::string line;
  std::getline(file, line);
  return true;
}

std::optional<DataEvent> readDataCsv::newBar()
{
  if (barIndex >= barBatch.size())
  {
    barBatch = readBinanceCsv();
    barIndex = 0;
  }

  if (barBatch.empty())
  {
    return std::nullopt;
  }

  return DataEvent(barBatch[barIndex++]);
}
