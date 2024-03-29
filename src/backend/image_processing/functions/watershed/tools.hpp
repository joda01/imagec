/** Returns a sorted list of indices of the specified double array.
  Modified from: http://stackoverflow.com/questions/951848 by N.Vischer.
*/

#include <algorithm>
#include <cstddef>
inline int *rank(double *values, size_t length)
{
  int n        = length;
  int *indexes = new int[n];
  double *data = new double[n];
  for(int i = 0; i < n; i++) {
    indexes[i] = i;
    data[i]    = values[i];
  }
  /* Arrays.sort(indexes, new Comparator<Integer>() {
     public int compare(final Integer o1, final Integer o2) {
       return data[o1].compareTo(data[o2]);}});*/

  std::sort(indexes, indexes + n, [&data](int o1, int o2) { return data[o1] < data[o2]; });
  int *indexes2 = new int[n];
  for(int i = 0; i < n; i++) {
    indexes2[i] = indexes[i];
  }
  return indexes2;
}
