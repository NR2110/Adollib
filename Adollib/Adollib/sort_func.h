#pragma once

namespace Adollib {

	class Sortfunc {
	private:

#pragma region quicksort

		template<typename T>
		static void swap_(T* a, T* b)
		{
			T t = *a;
			*a = *b;
			*b = t;
		}

		template<typename T>
		static int partition_AN(T array[], float nums[], int l, int r) {
			int pivot = nums[r];
			int i = (l - 1);

			for (int j = l; j <= r - 1; j++) {
				if (nums[j] <= pivot) {
					i++;
					swap_<T>(&array[i], &array[j]);
					swap_<float>(&nums[i], &nums[j]);
				}
			}
			swap_<T>(&array[i + 1], &array[r]);
			swap_<float>(&nums[i + 1], &nums[r]);
			return (i + 1);
		}
		//::::
		template<typename T>
		static void quicksort_AN(T array[], float nums[], int l, int r) {
			if (l < r) {
				int pivot = partition_AN(array, nums, l, r);
				quicksort_AN<T>(array, nums, l, pivot - 1);
				quicksort_AN<T>(array, nums, pivot + 1, r);
			}
		}


		template<typename T>
		static int partition_AA(T array[], int l, int r) {
			int pivot = array[r];
			int i = (l - 1);

			for (int j = l; j <= r - 1; j++) {
				if (array[j] <= pivot) {
					i++;
					swap_<T>(&array[i], &array[j]);
				}
			}
			swap_<T>(&array[i + 1], &array[r]);
			return (i + 1);
		}
		template<typename T>
		static void quicksort_AA(T array[], int l, int r) {
			if (l < r) {
				int pivot = partition_AA(array, l, r);
				quicksort_AA<T>(array, l, pivot - 1);
				quicksort_AA<T>(array, pivot + 1, r);
			}
		}
#pragma endregion

	public:
		template<typename T>
		// T[]      : ソートを行う要素
		// float[]  : ソートに用いる要素
		// int count : 配列のcount
		static void quicksort(T array[], float nums[], int count) {

			if(array != nums)
				quicksort_AN<T>(array, nums, 0, count);
			else 
				quicksort_AA<T>(array, 0, count);

			return;
		}





	};
}