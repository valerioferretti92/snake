#include "score_entry.h"

using namespace std;

score_entry getScoreEntry(string _text, unsigned int _value){
  score_entry sentry;
  sentry.text = _text;
  sentry.value = _value;
  return sentry;
}
