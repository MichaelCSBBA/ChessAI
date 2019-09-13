#ifndef SUBJECT_H
#define SUBJECT_H
#include <vector>
#include <memory>

template <typename InfoType, typename StateType> class Observer;

template <typename InfoType, typename StateType> class Subject {
  std::vector<std::weak_ptr<Observer<InfoType, StateType>>> observers;
 public:
  void attach(std::weak_ptr<Observer<InfoType, StateType>> o);  
  void notifyObservers(const StateType &state);
  virtual InfoType getInfo() const = 0;
};

template <typename InfoType, typename StateType>
void Subject<InfoType, StateType>::attach(std::weak_ptr<Observer<InfoType, StateType>> o) {
  observers.emplace_back(o);
}

template <typename InfoType, typename StateType>
void Subject<InfoType, StateType>::notifyObservers(const StateType &state) {
  for (auto &ob : observers) ob.lock()->notify(*this, state);
}
#endif
