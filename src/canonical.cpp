#include <iostream>

#include <string.h>
#include <stdlib.h>

#include "brickvector.h"

using namespace std;


int main() {
  int N = -1, K = -1;

  cin >> N >> K;

  int last_id = -100;
  int drawings_read = 0;

  CFINT vector[1024];
  for (int i = 0; i < 1024; i++)
    vector[i] = 0;

  while (!cin.eof()) {
    int id, crossingcount;
    cin >> id >> crossingcount;
    if (cin.eof())
      break;

    if (id <= last_id) {
      cerr << "Drawing id's are not increasing -- perhaps a reading error?" << endl;
      cerr << "Current id: " << id << ", previous id: " << last_id << endl;
      return -1;
    }

    // vector consists of 4 crossingcount more elements
    int len = 5 + 4 * crossingcount;
    vector[0] = N;
    vector[1] = K;
    vector[2] = 0;
    vector[3] = 0;
    vector[4] = crossingcount;
    for (int i = 5; i < len; i++)
      cin >> vector[i];

    calc_canonical(vector, false);
    print_vector(vector);

    last_id = id;
    drawings_read++;


#ifdef SHOWPROGRESS
    if (drawings_read % 1000 == 0)
      cerr << drawings_read << " drawings read" << endl;
#endif
  }

}
