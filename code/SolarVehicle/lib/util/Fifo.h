/***************************************************************************
 * @file    Fifo.h                                                         *
 * @authors Steffan Martin Kunoy and Tjark Petersen                        *
 * @brief   Declaration of Fifo typeclass, which implements a FIFO buffer. *
 ***************************************************************************/ 

#ifndef __FIFO_H__
#define __FIFO_H__
#include <ChRt.h>
 
template<typename T>    // Typeclass declaration means that Fifo can contain any object type. 
class Fifo 
{

    /* <!-- Private member variables --> */
    private: 
        semaphore_t spaceSem, dataSem;
        T *fifo;
        size_t _size, _head, _tail;  

    /* <!-- Public member functions --> */
    /**
     * @brief   Fifo constructor function.           
     * @param   size Number of elements in the Fifo. 
    */ 
    public: Fifo(uint32_t size)
    {
        this->_size = size;
        this->_head = 0;
        this->_tail = 0;
        spaceSem    = _SEMAPHORE_DATA(spaceSem,(cnt_t)size);
        dataSem     = _SEMAPHORE_DATA(dataSem,(cnt_t)0);
        fifo        = new T[size];
    }

    /**
     * @brief   Deletes the current instance of the Fifo class. 
     * @param   None                                            
    */
    public: ~Fifo()
    {
        delete [] fifo;
    }

    /**
     * @brief   Function to access element at a certain index in the FIFO. 
     * @param   index The index of the element to be retrieved.            
     * @return  A pointer to the element at the given index in the FIFO.   
    */
    public: T *get(uint32_t index)
    {
        return &fifo[index];
    }

    /**
     * @brief   Puts the caller thread to sleep until free fifo slots are present.
     * @param   None                                                               
     * @return  void                                                               
    */
    public: void waitForSpace()
    {
        chSemWait(&spaceSem);
    }

    /**
     * @brief   Puts the caller thread to sleep until new data is present. 
     * @param   None                                                       
     * @return  void                                                       
    */
    public: void waitForData()
    {
        chSemWait(&dataSem);
    }

    /**
     * @brief   Advances a FIFO pointer to the next available slot. 
     * @param   index The index of the FIFO slot to advance.        
     * @return  A pointer to the index of the next FIFO slot.       
    */
    public: uint32_t advance(uint32_t index)
    {
        return index < (_size - 1) ? index + 1 : 0;  
    }

    /**
     * @brief   Overloaded advance() function.                      
     * @param   index Pointer to an index of FIFO slot to advance.  
     * @return  void                                                
    */
    public: void advance(uint32_t *index)
    {
        *index = *index < (_size - 1) ? *index + 1 : 0;
    }

    /**
     * @brief   Signals that an element has been read and that the slot is free for new data. 
     * @param   None                                                                          
     * @return  void                                                                          
    */
    public: void signalSpace()
    {
        chSemSignal(&spaceSem);
    }

    /**
     * @brief   Signals that new data has been written to the FIFO. 
     * @param   None                                                
     * @return  void                                                
    */
    public: void signalData()
    {
        chSemSignal(&dataSem);
    }
    
    /**
     * @brief   Getter function to get the size, i.e. number of elements, of the FIFO. 
     * @param   None                                                                   
     * @return  The size of the FIFO buffer.                                           
    */
    public: uint32_t getSize()
    {
        return _size;
    }

    /**
     * @brief   Clears all data in the FIFO, set to 0. 
     * @param   None                                   
     * @return  void                                   
    */
    public: void clear()
    {
        memset(fifo, 0, _size * sizeof(T));
    }
    
    /**
     * @brief   Format prints all the elements in the FIFO buffer for debugging. 
     * @param   None                                                             
     * @return  void                                                             
    */
    public: void printContents()
    {
        for (int i = 0; i < _size; i++){
            Serial.print(i);
            Serial.print(" : ");
            char str[64];
            fifo[i].toString(str,64);
            Serial.println(str);
        }
    }
    
    /**
     * @brief   Getter function to extract the element at the head of the Fifo buffer. 
     * @param   None                                                                   
     * @return  A pointer to the element at the head index.                            
    */
    public: T *head()
    {
        return &fifo[_head];    
    }

    /**
     * @brief   Getter function to extract the element at the tail of the Fifo buffer. 
     * @param   None                                                                   
     * @return  A pointer to the element at the tail index.                            
    */
    public: T *tail()
    {
        return &fifo[_tail];
    }

    /**
     * @brief   Setter function to increment the head index of the Fifo buffer. 
     * @param   None                                                            
     * @return  void                                                            
    */
    public: void moveHead()
    {
        _head = advance(_head); 
    }

    /**
     * @brief   Setter function to increment the tail index of the Fifo buffer. 
     * @param   None                                                            
     * @return  A pointer to the element at the head index.                     
    */
    public: void moveTail()
    {
        _tail = advance(_tail);
        if(_tail == _head){
            _head = advance(_head);
        }
    }

    /**
     * @brief   Function used to check whether the Fifo buffer has space for new elements. 
     * @param   None                                                                        
     * @return  True if the buffer is full, otherwise false.                                
    */
    public: bool full()
    {
        return advance(_tail) == _head;
    }

    /**
     * @brief   Function used to check whether the Fifo buffer is empty. 
     * @param   None                                                                        
     * @return  True if the buffer is empty, otherwise false.                                
    */
    public: bool empty()
    {
        return _tail == _head; 
    }
};

#endif