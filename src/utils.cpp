#include <iomanip>
#include <chrono>

using namespace std;

string generateIdentifier(){
  unsigned long long int millsecondsfromEpoch;

  millsecondsfromEpoch = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch()).count();
  return to_string(millsecondsfromEpoch);
}

string getTimestamp() {
  const auto now = std::chrono::system_clock::now();
  const auto nowAsTimeT = std::chrono::system_clock::to_time_t(now);

  std::stringstream timestamp;
  timestamp << std::put_time(std::localtime(&nowAsTimeT), "%a %b %d %Y %T");
  return timestamp.str();
}
