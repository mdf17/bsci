#ifndef THREADSAFEQUEUE_H
#define THREADSAFEQUEUE_H

#include <memory>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QQueue>

const unsigned int MAX_QUEUE_SIZE = 10000;

template<typename T, typename C = QQueue<T> >
class ThreadSafeQueue
{
  public:
    ThreadSafeQueue() : m_queueBudget(MAX_QUEUE_SIZE) { }
    ThreadSafeQueue(unsigned int maxQueueSize) : m_queueBudget(maxQueueSize), m_size(0) { }

    inline void broadcastNewDataAvailable();
    inline void broadcastQueueNotFull();
    void clear();
    void push(const typename C::value_type &element);
    void pushBlocking(const typename C::value_type &element);
    bool getElementNonblocking(typename C::value_type &element_return);

    bool empty() {
        QMutexLocker locker(&mtx);
        return m_queue.isEmpty();
    }

    int size() { 
        QMutexLocker locker(&mtx);
        return m_queue.size(); 
    }

    void setQueueBudget(unsigned int budget) {
        QMutexLocker locker(&mtx);
        m_queueBudget = budget;
    }

  private:
    QMutex mtx;
    QWaitCondition qNewDataCond;
    QWaitCondition qNotFullCond;
    C m_queue;
    unsigned int m_queueBudget;
    unsigned int m_size;
};


template<typename T, typename C>
void ThreadSafeQueue<T, C>::broadcastNewDataAvailable()
{
    mtx.lock();
    m_size++;
    qNewDataCond.wakeAll();
    mtx.unlock();
}

template<typename T, typename C>
void ThreadSafeQueue<T, C>::broadcastQueueNotFull()
{
    mtx.lock();
    qNotFullCond.wakeAll();
    mtx.unlock();
}

/** Thread safe non-blocking push(). */
template<typename T, typename C>
void ThreadSafeQueue<T, C>::push(const typename C::value_type &element)
{
    mtx.lock();
    m_queue.push_back(element);
    m_size++;
    if( m_size > m_queueBudget )
        m_queueBudget = m_size;

    mtx.unlock();
}


/** Thread safe blocking push(). */
template<typename T, typename C>
void ThreadSafeQueue<T, C>::pushBlocking(const typename C::value_type &element)
{
    mtx.lock();
    if ( m_size >= m_queueBudget )
        qNotFullCond.wait(&mtx);
    mtx.unlock();

    m_queue.push_back(element);
    m_size++;

    broadcastNewDataAvailable();
}

/**
   Thread safely remove all elements from the queue
*/
template<typename T, typename C>
void ThreadSafeQueue<T, C>::clear()
{
    mtx.lock();
    m_queue.clear();
    m_size = 0;
    qNotFullCond.wakeAll();
    mtx.unlock();
}


/**
   Thread safely gets the next element if the queue is non-empty.
   @returns true if successful.
   @returns false if the queue is empty.
*/
template<typename T, typename C>
bool ThreadSafeQueue<T, C>::getElementNonblocking(typename C::value_type
                                                    &element_return)
{
    bool success = false;

    typename C::value_type val;

    mtx.lock();
    if( m_size > 0 )
    {
        val = m_queue.front();
        m_queue.pop_front();
        m_size--;

        success = true;
    }

    mtx.unlock();

    if(success) 
      element_return=val;

    return success;
}


template<typename T> 
using SharedQueue = std::shared_ptr<ThreadSafeQueue<T>>;

#endif
