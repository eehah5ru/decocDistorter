#pragma once

#include <mutex>
#include <queue>
#include <condition_variable>
#include <memory>
#include <optional>

template<typename T>
class myThreadChannel{
public:
  myThreadChannel()
  :closed(false){}

  std::optional<std::shared_ptr<T>> receive(){
    std::unique_lock<std::mutex> lock(mutex);
    if(closed){
      return {};
    }
    while(queue.empty() && !closed){
      condition.wait(lock);
    }
    if(!closed){
      auto r = queue.front();
      queue.pop();
      return {r};
    }else{
      return {};
    }
  }

  bool tryReceive(std::shared_ptr<T> sentValue){
    std::unique_lock<std::mutex> lock(mutex);
    if(closed){
      return false;
    }
    if(!queue.empty()){
      std::swap(sentValue,queue.front());
      queue.pop();
      return true;
    }else{
      return false;
    }
  }

  bool tryReceive(std::shared_ptr<T> sentValue, int64_t timeoutMs){
    std::unique_lock<std::mutex> lock(mutex);
    if(closed){
      return false;
    }
    if(queue.empty()){
      condition.wait_for(lock, std::chrono::milliseconds(timeoutMs));
      if(queue.empty()) {
        return false;
      }
    }

    if(!closed){
      std::swap(sentValue,queue.front());
      queue.pop();
      return true;
    }else{
      return false;
    }
  }

  bool send(const std::shared_ptr<T> value){
    std::unique_lock<std::mutex> lock(mutex);
    if(closed){
      return false;
    }
    queue.push(value);
        condition.notify_one();
    return true;
  }

  // bool send(std::shared_ptr<T> & value){
  //   std::unique_lock<std::mutex> lock(mutex);
  //   if(closed){
  //     return false;
  //   }
  //   queue.push(std::move(value));
  //       condition.notify_one();
  //   return true;
  // }

  void close(){
    std::unique_lock<std::mutex> lock(mutex);
    closed = true;
    condition.notify_all();
  }

  void clear() {
    if (!queue.empty()) {
      std::unique_lock<std::mutex> lock(mutex);
      queue = {};
    }
  }

  bool empty() const{
    return queue.empty();
  }


  size_t size() const {
    return queue.size();
  }

private:
  /// \brief The FIFO data queue.
  std::queue<std::shared_ptr<T>> queue;

  /// \brief The mutext to protect the data.
  std::mutex mutex;

  /// \brief The condition even to notify receivers.
  std::condition_variable condition;

  /// \brief True if the channel is closed.
  bool closed;

};
