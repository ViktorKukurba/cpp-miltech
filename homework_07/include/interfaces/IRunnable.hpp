#pragma once

// IRunnable — interface for objects that can be started and stopped in a background thread.
class IRunnable {
public:
  virtual ~IRunnable() = default;
  virtual bool start() = 0;
  virtual bool stop() = 0;
  virtual bool isThreadReady() = 0;
  virtual void run() = 0;
};
