// This file is part of UFAL C++ Utils <http://github.com/ufal/cpp_utils/>.
//
// Copyright 2017 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#pragma once

#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <queue>

#include "common.h"

namespace ufal {
namespace udpipe {
namespace utils {

//
// Declarations
//

template <class T>
class threadsafe_resource_loader {
 public:
  threadsafe_resource_loader(unsigned concurrent_limit) : concurrent_limit(concurrent_limit) {}

  unsigned add(T* resource);

  T* load(unsigned id);
  void release(unsigned id);

 private:
  unsigned concurrent_limit;

  struct resource_info {
    enum resource_state { NOT_LOADED, LOADING, LOADED};

    T* resource;
    unsigned used_count;
    resource_state state;

    resource_info(T* resource) : resource(resource), used_count(0), state(NOT_LOADED) {}
  };
  vector<resource_info> resources;
  vector<unsigned> loaded_resources;
  unsigned used_resources = 0;
  queue<unsigned> queued_resources;

  mutex resource_mutex;
  condition_variable resource_cv;
};

//
// Definitions
//

template <class T>
unsigned threadsafe_resource_loader<T>::add(T* resource) {
  resources.push_back(resource);
  return resources.size() - 1;
}

template <class T>
T* threadsafe_resource_loader<T>::load(unsigned id) {
  if (id < resources.size()) {
    unique_lock<mutex> lock(resource_mutex);

    if (!resources[id].used_count++) {
      if (resources[id].state == resource_info::LOADED) {
        loaded_resources.erase(remove(loaded_resources.begin(), loaded_resources.end(), id), loaded_resources.end());
        loaded_resources.push_back(id);
        used_resources++;
        return resources[id].resource;
      }

      // Load the resource
      resources[id].state = resource_info::LOADING;
      queued_resources.push(id);
      while (used_resources >= concurrent_limit || queued_resources.front() != id)
        resource_cv.wait(lock);

      queued_resources.pop();
      if (loaded_resources.size() == concurrent_limit)
        for (size_t i = 0; i < loaded_resources.size(); i++)
          if (!resources[loaded_resources[i]].used_count) {
            resources[loaded_resources[i]].resource->release();
            resources[loaded_resources[i]].state = resource_info::NOT_LOADED;
            loaded_resources.erase(loaded_resources.begin() + i);
            break;
          }
      loaded_resources.push_back(id);
      used_resources++;

      lock.unlock();
      bool loaded = resources[id].resource->load();
      lock.lock();

      if (loaded) {
        resources[id].state = resource_info::LOADED;
      } else {
        resources[id].state = resource_info::NOT_LOADED;
        resources[id].used_count--;
        used_resources--;
        loaded_resources.erase(remove(loaded_resources.begin(), loaded_resources.end(), id), loaded_resources.end());
      }

      lock.unlock();
      resource_cv.notify_all();

      return loaded ? resources[id].resource : nullptr;
    } else {
      // Wait until the model is loaded if required
      while (resources[id].state == resource_info::LOADING)
        resource_cv.wait(lock);

      if (resources[id].state == resource_info::LOADED) {
        // Move the resource to the end of the resource_queue
        loaded_resources.erase(remove(loaded_resources.begin(), loaded_resources.end(), id), loaded_resources.end());
        loaded_resources.push_back(id);
      } else {
        resources[id].used_count--;
      }

      return resources[id].state == resource_info::LOADED ? resources[id].resource : nullptr;
    }
  }

  return nullptr;
}

template <class T>
void threadsafe_resource_loader<T>::release(unsigned id) {
  bool notify_all = false;

  if (id < resources.size()) {
    unique_lock<mutex> lock(resource_mutex);

    if (resources[id].used_count)
      if (!--resources[id].used_count)
        notify_all = used_resources-- == concurrent_limit;
  }

  if (notify_all)
    resource_cv.notify_all();
}

} // namespace utils
} // namespace udpipe
} // namespace ufal
