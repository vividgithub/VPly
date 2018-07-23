//
// Created by vivi on 03/04/2018.
//

#pragma once

#include "vplyutil.h"

namespace VPly {

	enum PrimitiveID {
		Point = 1,
		Line = 2,
		Sphere = 3,
		UniformGrid = 4,
		Box = 5,
		AABB = 6
	};

	inline void writeVPlyAttribute(std::ostream & os) {
		int32_t zero = 0;
		os.write((char*)(&zero), sizeof(int32_t));
	}

	template<typename T, int Size, class ...Attributes>
	inline void writeVPlyAttribute(std::ostream & os, const VPlyAttribute<VPlyVector<T, Size>> & attr, Attributes&& ...attributes) {
		attr.stream(os);
		writeVPlyAttribute(os, attributes...);
	};

	template<typename T, int Rows, int Cols, class ...Attributes>
	inline void writeVPlyAttribute(std::ostream & os, const VPlyAttribute<VPlyMatrix<T, Rows, Cols>> & attr, Attributes&& ...attributes) {
		attr.stream(os);
		writeVPlyAttribute(os, attributes...);
	};

	inline void writePrimitiveID(std::ostream & os, PrimitiveID primitiveID) {
		int32_t id = primitiveID;
		os.write((char*)(&id), sizeof(int32_t));
	}

	template <class ...Attributes>
	inline void writePoint(std::ostream & os, const VPlyVector3f & pos, Attributes&& ...attributes) {
		writePrimitiveID(os, PrimitiveID::Point);
		pos.stream(os);
		writeVPlyAttribute(os, attributes...);
	}

	template <class ...Attributes>
	inline void writeLine(std::ostream & os, const VPlyVector3f & p1, const VPlyVector3f & p2, Attributes&& ...attributes) {
		writePrimitiveID(os, PrimitiveID::Line);
		p1.stream(os);
		p2.stream(os);
		writeVPlyAttribute(os, attributes...);
	}

	template <class ...Attributes>
	inline void writeSphere(std::ostream & os, const VPlyVector3f & center, const VPlyFloat & radius, const VPlyMatrix4f & transform, Attributes&& ...attributes) {
		writePrimitiveID(os, PrimitiveID::Sphere);
		center.stream(os);
		radius.stream(os);
		transform.stream(os);
		writeVPlyAttribute(os, attributes...);
	}

	template <class ...Attributes>
	inline void writeUniformGrid(std::ostream & os, const VPlyMatrix4f &transform, Attributes&& ...attributes) {
		writePrimitiveID(os, PrimitiveID::UniformGrid);
		transform.stream(os);
		writeVPlyAttribute(os, attributes...);
	}

	template <class ...Attributes>
	inline void writeBox(std::ostream & os, const VPlyMatrix4f &transform, Attributes &&...attributes) {
		writePrimitiveID(os, PrimitiveID::Box);
		transform.stream(os);
		writeVPlyAttribute(os, attributes...);
	}

	template <class ...Attributes>
	inline void writeAABB(std::ostream & os, const VPlyVector3f & corner1, const VPlyVector3f & corner2, Attributes &&...attributes) {
		writePrimitiveID(os, PrimitiveID::AABB);
		corner1.stream(os);
		corner2.stream(os);
		writeVPlyAttribute(os, attributes...);
	}
}
