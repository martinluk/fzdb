#ifndef SIMPLELOCK_H
#define SIMPLELOCK_H

#include <boost/thread/mutex.hpp>

// When constructed, the lock takes a reference to some property and a reference to a mutex.
// The mutex is locked on construction and unlocked on destruction.
// Const and non-const access to the property is available through the property accessors.
// If only const property access is required, objects returning these locks should return SimpleLock<const ...>.
// It is recommended to only keep the lock alive for as long as it is required, to relinquish the mutex
// as soon as possible for other threads.
// Keeping your own copy of the internal proprety reference held by the lock is dangerous, as when the lock dies the
// property is up for grabs for other threads again.

/* Eg:
 * {
 *   SimpleLock<int> lock = object.lockSomeProperty();	// Get a lock to the property. By the time it's returned the mutex is available.
 *   *lock = 5;											// Set the value.
 * }													// When the lock drops out of scope it is deleted and the mutex is released. 
 */

template<typename T>
class SimpleLock
{
public:
	SimpleLock(T &property, boost::mutex &mutex) :
		_mutex(mutex), _property(property)
	{
		_mutex.lock();
	}
	
	~SimpleLock()
	{
		_mutex.unlock();
	}
	
	T& property()
	{
		return _property;
	}
	
	const T& property() const
	{
		return _property;
	}
	
	// Operator overloads.
	T& operator *()
	{
		return _property;
	}
	
	const T& operator *() const
	{
		return _property;
	}
	
private:
	boost::mutex&	_mutex;
	T&				_property;
};

#endif // SIMPLELOCK_H
