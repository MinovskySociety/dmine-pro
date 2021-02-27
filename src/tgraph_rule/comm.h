#ifndef COMM_H
#define COMM_H

#include <mpi.h>

#include <limits>
#include <string>
#include <vector>

namespace toy {
// learn from grape

/* Init MPI, called when MPI starts */
inline void InitMPIComm() {
  int provided;
  MPI_Init_thread(NULL, NULL, MPI_THREAD_MULTIPLE, &provided);
}

/* Get MPI Info
 * @return: worker id, worker num
*/
inline void GetMPIInfo(int &worker_id, int &worker_num) {
  MPI_Comm_rank(MPI_COMM_WORLD, &worker_id);
  MPI_Comm_size(MPI_COMM_WORLD, &worker_num);
}

/* Finalize MPI, called when MPI ends */
inline void FinalizeMPIComm() { MPI_Finalize(); }

static const int chunk_size = 409600;

/* Send message in buffer */
template <typename T>
static inline void send_buffer(const T *ptr, size_t len, int dst_worker_id,
                               MPI_Comm comm, int tag) {
  const size_t chunk_size_in_bytes = chunk_size * sizeof(T);
  int iter = len / chunk_size;
  size_t remaining = (len % chunk_size) * sizeof(T);
  for (int i = 0; i < iter; ++i) {
    MPI_Send(ptr, chunk_size_in_bytes, MPI_CHAR, dst_worker_id, tag, comm);
    ptr += chunk_size;
  }
  if (remaining != 0) {
    MPI_Send(ptr, remaining, MPI_CHAR, dst_worker_id, tag, comm);
  }
}

/* Receive message in buffer */
template <typename T>
static inline void recv_buffer(T *ptr, size_t len, int src_worker_id,
                               MPI_Comm comm, int tag) {
  const size_t chunk_size_in_bytes = chunk_size * sizeof(T);
  int iter = len / chunk_size;
  size_t remaining = (len % chunk_size) * sizeof(T);
  for (int i = 0; i < iter; ++i) {
    MPI_Recv(ptr, chunk_size_in_bytes, MPI_CHAR, src_worker_id, tag, comm,
             MPI_STATUS_IGNORE);
    ptr += chunk_size;
  }
  if (remaining != 0) {
    MPI_Recv(ptr, remaining, MPI_CHAR, src_worker_id, tag, comm,
             MPI_STATUS_IGNORE);
  }
}

/* Send message by vector */
template <typename T>
inline void SendVector(const std::vector<T> &vec, int dst_worker_id,
                       MPI_Comm comm = MPI_COMM_WORLD, int tag = 0) {
  size_t len = vec.size();
  MPI_Send(&len, sizeof(size_t), MPI_CHAR, dst_worker_id, tag, comm);
  send_buffer<T>(&vec[0], len, dst_worker_id, comm, tag);
}

/* Receive message by vector */
template <typename T>
inline void RecvVector(std::vector<T> &vec, int src_worker_id,
                       MPI_Comm comm = MPI_COMM_WORLD, int tag = 0) {
  size_t len;
  MPI_Recv(&len, sizeof(size_t), MPI_CHAR, src_worker_id, tag, comm,
           MPI_STATUS_IGNORE);
  vec.resize(len);
  recv_buffer<T>(&vec[0], len, src_worker_id, comm, tag);
}

}  // namespace toy

#endif  // COMM_H
