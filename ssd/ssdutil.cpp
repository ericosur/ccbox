#include "ssdutil.h"

bool is_dog(int label_id)
{
    if (label_id == 8 || label_id == 12) {
        return true;
    } else {
        return false;
    }
}

bool is_person(int label_id)
{
    // person
    if (label_id == 15) {
        return true;
    } else {
        return false;
    }
}

std::string get_label_name(int label_id)
{
  std::string result;
  switch (label_id) {
    case 8: // cat
      result = "dog";
      break;
    case 12:  // dog
      result = "dog";
      break;
    case 15:  // person
      result = "man";
      break;
    default:
      result = "null";
      break;
  }
  return result;
}

