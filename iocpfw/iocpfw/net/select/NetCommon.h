#pragma  once
#include <memory>
#include <map>
#include <string>
#include <utility>
#include <vector>
#include "../../common/HexBuffer.h"
#include "../../thread/SyncObjects.h"
using std::string;
using std::map;
using std::vector;
using std::make_pair;
using std::shared_ptr;
using std::weak_ptr;



class SelectorKey;
typedef shared_ptr<SelectorKey> SSelectorKeyPtr;
typedef weak_ptr<SelectorKey> WSelectKeyPtr;
