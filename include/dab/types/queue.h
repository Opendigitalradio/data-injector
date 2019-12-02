/*
 * Copyright (C) 2017 Opendigitalradio (http://www.opendigitalradio.org/)
 * Copyright (C) 2017 Felix Morgner <felix.morgner@hsr.ch>
 * Copyright (C) 2017 Tobias Stauber <tobias.stauber@hsr.ch>
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 *    may be used to endorse or promote products derived from this software without
 *    specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DABCOMMON_TYPES_QUEUE
#define DABCOMMON_TYPES_QUEUE

#include <algorithm>
#include <array>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstring>
#include <memory>
#include <mutex>
#include <type_traits>
#include <utility>
#include <vector>

#ifdef CUTE_TESTING
namespace dab { namespace test { namespace type { namespace queue {
struct enqueueing_tests;
struct dequeueing_tests;
}}}}
#endif

namespace dab
  {

  namespace internal
    {

    /**
     * The default block size used by the queue
     *
     * @author Felix Morgner
     * @since  1.0.1
     */
    //std::size_t constexpr kQueueDefaultBlockSize{8192};
    std::size_t constexpr kQueueDefaultBlockSize{8};

    /**
     * The default number of blocks in a block group
     *
     * @author Felix Morgner
     * @since  1.0.1
     */
    //std::size_t constexpr kQueueDefaultGroupSize{512};
    std::size_t constexpr kQueueDefaultGroupSize{4};

    /**
     * @internal
     * @brief A thread-safe block-allocated SPSC queue
     *
     * This thread-safe queue is designed for SPSC scenarios. It manages memory in a blockwise fashion. The block size, as well
     * as the desired block group size can be specified as template parameters.
     *
     * @tparam ValueType The type of the elements contained in queue
     * @tparam BlockSize The size of the blocks allocated by the queue
     * @tparam GroupSize The size of the block allocation steps
     *
     * @author Felix Morgner
     * @since  1.0.1
     */
    template<typename ValueType, std::size_t BlockSize = kQueueDefaultBlockSize, std::size_t GroupSize = kQueueDefaultGroupSize>
    struct queue
      {
      using value_type = ValueType;
      using pointer = value_type *;
      using const_pointer = value_type const *;
      using reference = value_type &;
      using const_reference = value_type const &;

      static auto constexpr block_size = BlockSize;
      static auto constexpr group_size = GroupSize;
      static auto constexpr alloc_size = block_size * group_size;

      /**
       * @brief Construct an empty queue
       *
       * @param nofInitialGroups The number of block groups to preallocate during construction
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      explicit queue(std::size_t const nofInitialGroups = 1)
        : m_capacity{nofInitialGroups * alloc_size}
        , m_backingStore{allocate(m_capacity)}
        {

        }

      /**
       * @brief Get the current number of elements in the queue
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      std::size_t size() const
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        return m_size;
        }

      /**
       * @brief Get the approximate number of elements in the queue
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      std::size_t approximate_size() const
        {
        return m_size;
        }

      /**
       * @brief Enqueue a single element into the queue
       *
       * @note This call blocks until the element can be enqueued.
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      void enqueue(ValueType const & elem)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        do_enqueue(elem);
        }

      /**
       * @brief Enqueue a single element into the queue
       *
       * @note This call blocks until the element can be enqueued.
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      void enqueue(ValueType && elem)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        do_enqueue(std::move(elem));
        }

      /**
       * @brief Enqueue an arbitrarily sized block of elements into the queue
       *
       * @note This call blocks until the block can be enqueued.
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      void enqueue(std::vector<ValueType> const & block)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        do_enqueue_block(block);
        }

      /**
       * @brief Enqueue an arbitrarily sized block of elements into the queue
       *
       * @note This call blocks until the block can be enqueued.
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      void enqueue(std::vector<ValueType> && block)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        do_enqueue_block(std::move(block));
        }

      /**
       * @brief Dequeue a single element from the queue
       *
       * @note This call blocks until the element can be dequeued
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      void dequeue(ValueType & target)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        m_hasElements.wait(lock, [&]{ return m_size > 0; });

        do_dequeue(target);
        }

      /**
       * @brief Dequeue a block of elements from the queue
       *
       * @note This call blocks until the block can be dequeued
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      void dequeue(std::vector<ValueType> & block)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        m_hasElements.wait(lock, [&]{ return m_size >= block.size(); });

        do_dequeue_block(block);
        }


      /**
       * @brief Try to dequeue an element from the queue
       *
       * @note This call never blocks
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      bool try_dequeue(ValueType & target)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};

        if(!m_size)
          {
          return false;
          }

        do_dequeue(target);
        return true;
        }

      /**
       * @brief Try to dequeue a block of elements from the queue
       *
       * @note This call never blocks
       *
       * @author Felix Morgner
       * @since  1.0.1
       */
      bool try_dequeue(std::vector<ValueType> & block)
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};

        if(m_size < block.size())
          {
          return false;
          }

        do_dequeue_block(block);
        return true;
        }

      /**
       * @brief Clear the contents of the queue
       *
       * @since 1.0.3
       */
      void clear()
        {
        auto lock = std::unique_lock<std::mutex>{m_mutex};
        m_size = m_current = 0;
        }

      private:
#ifdef CUTE_TESTING
        friend dab::test::type::queue::enqueueing_tests;
        friend dab::test::type::queue::dequeueing_tests;
#endif

        /**
         * @internal
         * @brief The size in chars of the element type of the queue
         */
        static auto constexpr element_size = sizeof(value_type);

        /**
         * @internal
         * @brief A SFINAE type-wrapper to detect trivially-copyable types
         */
        template<typename Type>
        using detect_trivially_copyable = typename std::enable_if<
                                            std::is_trivially_copyable<
                                              Type
                                            >::value
                                          >::type;

        /**
         * @internal
         * @brief A SFINAE type-wrapper to detect non-trivially-copyable types
         */
        template<typename Type>
        using detect_non_trivially_copyable = typename std::enable_if<
                                                !std::is_trivially_copyable<
                                                  Type
                                                >::value
                                              >::type;

        /**
         * @internal
         * @brief Worker function for enqueueing a single element
         *
         * @pre  size() > 0
         * @post size() == OLD(size()) - 1
         * @note This function does **NOT** lock the queue!
         *
         * @author Felix Morgner
         * @since  1.0.1
         */
        template<typename ElementType>
        void do_enqueue(ElementType && element)
          {
          if(!available())
            {
            resize(m_capacity + alloc_size);
            }

          new (enqueue_pointer()) value_type{std::forward<ElementType>(element)};

          m_size++;
          m_hasElements.notify_one();
          }

        /**
         * @internal
         * @brief Worker function for enqueueing a block of elements
         *
         * @pre  size() >= block.size()
         * @post size() == OLD(size()) - block.size()
         * @note This function does **NOT** lock the queue!
         *
         * @since  1.0.1
         */
        template<typename BlockType>
        void do_enqueue_block(BlockType && block)
          {
          auto const blockSize = block.size();
          auto const availableSlots = available();

          if(blockSize >= availableSlots)
            {
            auto factor = (blockSize - availableSlots) / alloc_size + 1;
            resize(m_capacity + factor * alloc_size);
            }

          do_enqueue_block_impl<ValueType>(std::forward<BlockType>(block));

          m_size += blockSize;
          m_hasElements.notify_one();
          }

        /**
         * @internal
         * @brief Concrete block enqueueing implementation for trivially-copyable element types
         *
         * @since 1.0.1
         */
        template<typename DependentType, typename BlockType>
        detect_trivially_copyable<DependentType> do_enqueue_block_impl(BlockType && block)
          {
          auto target = enqueue_pointer();
          std::memcpy(target, block.data(), block.size() * element_size);
          }

        /**
         * @internal
         * @brief Concrete block enqueueing implementation for non-trivially-copyable element types
         *
         * @since 1.0.1
         */
        template<typename DependentType, typename BlockType>
        detect_non_trivially_copyable<DependentType> do_enqueue_block_impl(BlockType && block)
          {
          auto target = dequeue_pointer() + m_size;
          for(auto && entry : block)
            {
            new (target++) value_type{std::move(entry)};
            }
          }

        /**
         * @internal
         * @brief Worker function for dequeueing an element from the queue
         *
         * @pre  size() > 0
         * @post size() == OLD(size()) - 1
         * @note This function does **NOT** lock the queue
         *
         * @author Felix Morgner
         * @since  1.0.1
         */
        void do_dequeue(ValueType & target)
          {
          target = std::move(*dequeue_pointer());
          dequeue_pointer()->~value_type();

          --m_size;
          ++m_current;

          if(m_current > m_capacity / 2)
            {
            shift_down();
            }
          }

        /**
         * @internal
         * @brief Worker function for dequeueing a block of elements from the queue
         *
         * @pre  size() >= block.size()
         * @post size() == OLD(size()) - block.size()
         * @note This function does not lock the queue
         *
         * @author Felix Morgner
         * @since  1.0.1
         */
        template<typename BlockType>
        void do_dequeue_block(BlockType & block)
          {
          do_dequeue_block_impl<ValueType, BlockType>(block);

          m_size -= block.size();
          m_current += block.size();

          if(m_current > m_capacity / 2)
            {
            shift_down();
            }
          }

        /**
         * @internal
         * @brief Block dequeueing implementation for trivially-copyable element types
         *
         * @since 1.0.1
         */
        template<typename DependentType, typename BlockType>
        detect_trivially_copyable<DependentType> do_dequeue_block_impl(BlockType & block)
          {
          std::memcpy(block.data(), dequeue_pointer(), block.size() * sizeof(ValueType));
          }

        /**
         * @internal
         * @brief Block dequeueing implementation for non-trivially-copyable element types
         *
         * @since 1.0.1
         */
        template<typename DependentType, typename BlockType>
        detect_non_trivially_copyable<DependentType> do_dequeue_block_impl(BlockType & block)
          {
          auto source = dequeue_pointer();
          for(auto & entry : block)
            {
            entry = std::move(*source);
            (source++)->~value_type();
            }
          }

        /**
         * @internal
         * @brief Move the contained items down to the lower end of the backing store
         *
         * @since 1.0.1
         */
        void shift_down()
          {
          shift_down_impl<value_type>();
          m_current = 0;
          }

        /**
         * @internal
         * @brief Concrete element shifting implementation for trivially-copyable element types
         *
         * @since 1.0.1
         */
        template<typename Dependent>
        detect_trivially_copyable<Dependent> shift_down_impl()
          {
          std::memmove(base_pointer(), dequeue_pointer(), m_size * sizeof(ValueType));
          }

        /**
         * @internal
         * @brief Concrete element shifting implementation for non-trivially-copyable element types
         *
         * @since 1.0.1
         */
        template<typename Dependent>
        detect_non_trivially_copyable<Dependent> shift_down_impl()
          {
          for(std::size_t idx{}; idx < m_size; ++idx)
            {
            auto const sourcePointer = dequeue_pointer() + idx;
            new (base_pointer() + idx) value_type{std::move(*sourcePointer)};
            sourcePointer->~value_type();
            }
          }

        /**
         * @internal
         * @brief Resize the backing store to fit the given number of elements
         *
         * @since 1.0.1
         */
        void resize(std::size_t const elements)
          {
          auto newBackingStore = allocate(elements);

          transfer<value_type>(newBackingStore);

          m_backingStore = std::move(newBackingStore);
          m_current = 0;
          m_capacity = elements;
          }

        /**
         * @internal
         * @brief Concrete implementation of the element transfer for trivially-copyable element type
         *
         * @since 1.0.1
         */
        template<typename Dependent>
        detect_trivially_copyable<Dependent> transfer(std::unique_ptr<char[]> & targetStore)
          {
          std::memcpy(targetStore.get(), dequeue_pointer(), element_size * m_size);
          }

        /**
         * @internal
         * @brief Concrete implementation of the element transfer for non-trivially-copyable element type
         *
         * @since 1.0.1
         */
        template<typename Dependent>
        detect_non_trivially_copyable<Dependent> transfer(std::unique_ptr<char[]> & targetStore)
          {
          auto target = reinterpret_cast<value_type *>(targetStore.get());

          for(std::size_t idx{}; idx < m_size; ++idx, ++target)
            {
            auto const sourcePointer = dequeue_pointer() + idx;
            new (target) value_type{std::move(*sourcePointer)};
            sourcePointer->~value_type();
            }
          }

        /**
         * @internal
         * @brief Allocate a memory region for a given number of elements
         *
         * @since 1.0.1
         */
        std::unique_ptr<char[]> allocate(std::size_t elements)
          {
          return std::unique_ptr<char[]>{new char[element_size * elements]};
          }

        /**
         * @internal
         * @brief Get a pointer to the first element to be dequeued
         *
         * @since 1.0.1
         */
        pointer dequeue_pointer() const
          {
          return base_pointer() + m_current;
          }

        /**
         * @internal
         * @brief Get a pointer to slot where the first element can be enqueued
         */
        pointer enqueue_pointer() const
          {
          return dequeue_pointer() + m_size;
          }

        /**
         * @internal
         * @brief Get a pointer to the base of the backing store
         *
         * @since 1.0.1
         */
        value_type * base_pointer() const
          {
          return reinterpret_cast<value_type *>(m_backingStore.get());
          }

        /**
         * @internal
         * @brief Get the number of free elements currently available
         *
         * @since 1.0.1
         */
        std::size_t available() const
          {
          return m_capacity - m_current - m_size;
          }

        std::atomic_size_t m_capacity{};
        std::atomic_size_t m_current{};
        std::atomic_size_t m_size{};
        std::unique_ptr<char[]> m_backingStore{};
        std::mutex mutable m_mutex{};
        std::condition_variable m_hasElements{};
      };

    }

  }

#endif
