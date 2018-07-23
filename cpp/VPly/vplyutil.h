//
// Created by vivi on 03/04/2018.
//

#pragma once
#include <iostream>
#include <array>
#include <algorithm>

namespace VPly {

	struct VPlyBase {
	public:
		//Write data to a stream value
		virtual void stream(std::ostream & os) const = 0;
	};

	namespace internal {
		template <typename T>
		constexpr int getVectorTypeID(int size) { return 0; };

		template <>
		constexpr int getVectorTypeID<int32_t>(int size) { return size; }

		template <>
		constexpr int getVectorTypeID<float>(int size) { return 32 + size; };

		template <typename T>
		constexpr int getMatrixTypeID(int size) { return 0; }

		template <>
		constexpr int getMatrixTypeID<float>(int size) {

			if (size == 3)
				return 257; //TypeID for Matrix3f
			else if (size == 4)
				return 258; //TypeID for Matrix4f

			return 0;
		};
	}

	template <typename T, int Size>
	struct VPlyVector: public VPly::VPlyBase {
	public:
		std::array<T, Size> data;

		enum {
			typeID = internal::getVectorTypeID<T>(Size)
		};

		VPlyVector() = default;

		template <typename = std::enable_if<Size == 1>>
		VPlyVector(const T & x): data {x} {}

		template <typename = std::enable_if<Size == 2>>
		VPlyVector(const T & x, const T & y):
				data {x, y} {}

		template <typename = std::enable_if<Size == 3>>
		VPlyVector(const T & x, const T & y, const T & z):
				data {x, y, z} {}

		template <typename = std::enable_if<Size == 4>>
		VPlyVector(const T & x, const T & y, const T & z, const T & w):
				data {x, y, z, w} {}

		void stream(std::ostream & os) const override {
			os.write((char*)data.data(), sizeof(T) * Size);
		}
	};

	using VPlyInt = VPlyVector<int32_t, 1>;
	using VPlyVector2i = VPlyVector<int32_t, 2>;
	using VPlyVector3i = VPlyVector<int32_t, 3>;
	using VPlyVector4i = VPlyVector<int32_t, 4>;

	using VPlyFloat = VPlyVector<float, 1>;
	using VPlyVector2f = VPlyVector<float, 2>;
	using VPlyVector3f = VPlyVector<float, 3>;
	using VPlyVector4f = VPlyVector<float, 4>;


	template <typename T, int Rows, int Cols>
	struct VPlyMatrix: public VPlyBase {
	public:
		enum {
			numSize = Rows * Cols,
			dataSize = Rows * Cols * sizeof(T),
			typeID = internal::getMatrixTypeID<T>(Rows)
		};

		std::array<T, numSize> data;

		VPlyMatrix() = default;
		VPlyMatrix(const std::initializer_list<T>& l) {
			if (numSize <= l.size()) {
				memcpy(data.data(), l.begin(), dataSize);
			}
			else if (l.size() > 0) {
				memcpy(data.data(), l.begin(), l.size() * sizeof(T));
				const auto & lastValue = *(l.end() - 1);
				for (size_t i = l.size(); i < numSize; ++i)
					data[i] = lastValue;
			}
		}

		template <typename = std::enable_if<Rows == Cols>>
		constexpr static VPlyMatrix identity() {
			VPlyMatrix m;

			for (size_t r = 0; r < Rows; ++r) {
				for (size_t c = 0; c < Cols; ++c)
					m.data[r * Rows + c] = 0;
				m.data[r * Rows + r] = 1;
			}

			return m;
		}


		void stream(std::ostream & os) const override  {
			os.write((char*)data.data(), dataSize);
		}
	};

	using VPlyMatrix3f = VPlyMatrix<float, 3, 3>;
	using VPlyMatrix4f = VPlyMatrix<float, 4, 4>;

	template <typename ValueType, typename = std::enable_if< std::is_base_of<VPly::VPlyBase, ValueType>::value> >
	struct VPlyAttribute: public VPlyBase {
		std::string key;
		ValueType value;

		VPlyAttribute(const std::string & key_, const ValueType & value_):
				key(key_),
				value(value_) {}

		VPlyAttribute(const std::string & key_, ValueType && value__):
				key(key_),
				value(std::move(value__)) {}

		VPlyAttribute(std::string && key__, const ValueType & value_):
				key(std::move(key__)),
				value(value_) {}

		VPlyAttribute(std::string && key__, ValueType && value__):
				key(std::move(key__)),
				value(std::move(value__)) {}

		VPlyAttribute(const VPlyAttribute & r_) = default;
		VPlyAttribute(const VPlyAttribute && r__):
				key(std::move(r__.key)),
				value(std::move(r__.value)) {}

		void stream(std::ostream & os) const override {
			int32_t keyLength = key.size();
			os.write((char*)(&keyLength), sizeof(int32_t));

			os << key;
			int32_t typeID_ = ValueType::typeID;
			os.write((char*)(&typeID_), sizeof(int32_t));
			value.stream(os);
		}
	};

	using VPlyIntAttr = VPlyAttribute<VPlyInt>;
	using VPlyVector2iAttr = VPlyAttribute<VPlyVector2i>;
	using VPlyVector3iAttr = VPlyAttribute<VPlyVector3i>;
	using VPlyVector4iAttr = VPlyAttribute<VPlyVector4i>;

	using VPlyFloatAttr = VPlyAttribute<VPlyFloat>;
	using VPlyVector2fAttr = VPlyAttribute<VPlyVector2f>;
	using VPlyVector3fAttr = VPlyAttribute<VPlyVector3f>;
	using VPlyVector4fAttr = VPlyAttribute<VPlyVector4f>;

	using VPlyMatrix3fAttr = VPlyAttribute<VPlyMatrix3f>;
	using VPlyMatrix4fAttr = VPlyAttribute<VPlyMatrix4f>;
}