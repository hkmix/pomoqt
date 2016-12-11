#ifndef POMORESULT_H
#define POMORESULT_H

#include <exception>
#include <string>

enum class PomoResultCode {
  SUCCESS,
  FAIL,
};

// Exception
class PomoResultException : public std::exception {
public:
  PomoResultException(const std::string& msg);
  PomoResultException(const char* msg);
  virtual const char* what() const noexcept;

private:
  const char* msg_;
};

PomoResultException::PomoResultException(const std::string& msg)
  : msg_(msg.c_str())
{
}

PomoResultException::PomoResultException(const char* msg)
  : msg_(msg)
{
}

const char* PomoResultException::what() const noexcept
{
  return msg_;
}

// PomoResult class
template<typename T>
class PomoResult {
public:
  PomoResult();
  PomoResult(PomoResultCode code, T value);
  PomoResult(PomoResultCode code, T&& value);

  bool successful() const;
  PomoResultCode code() const;
  const T& value() const;

private:
  const PomoResultCode code_;
  T value_;
};

template<typename T>
PomoResult<T>::PomoResult()
  : code_(PomoResultCode::FAIL)
{
}

template<typename T>
PomoResult<T>::PomoResult(PomoResultCode code, T value)
  : code_(code)
  , value_(value)
{
}

template<typename T>
PomoResult<T>::PomoResult(PomoResultCode code, T&& value)
  : code_(code)
  , value_(value)
{
}

/**
 * @brief Simple code check
 * @return Whether result is SUCCESS
 */
template<typename T>
bool PomoResult<T>::successful() const
{
  return code_ == PomoResultCode::SUCCESS;
}

/**
 * @brief Retrieve result code
 * @return PomoResultCode result
 */
template<typename T>
PomoResultCode PomoResult<T>::code() const
{
  return code_;
}

/**
 * @brief Retrieve value
 * @return Result value
 */
template <typename T>
const T& PomoResult<T>::value() const
{
  if (!successful()) {
    throw PomoResultException("Cannot access value of failed query.");
  }

  return value_;
}

#endif // POMORESULT_H
