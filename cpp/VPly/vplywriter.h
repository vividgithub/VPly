//
// Created by vivi on 03/04/2018.
//

#pragma once

#include <vector>
#include <sstream>
#include <type_traits>

#include "vplyutil.h"

namespace VPly {

	enum PrimitiveID {
		Point = 1,
		Line = 2,
		Sphere = 3,
		UniformGrid = 4,
		Box = 5,
		AABB = 6,
		AttributedLineStrip_ = 7
	};

	inline void writeVPlyAttribute(std::ostream & os) {
		int32_t zero = 0;
		os.write((char*)(&zero), sizeof(int32_t));
	}

	template<typename T, class ...Attributes>
	void writeVPlyAttribute(std::ostream & os, const VPlyAttribute<T> & attr, Attributes&& ...attributes) {
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

	/**
	 * An base class of Attributed Primitive
	 */
	class AttributedPrimitiveBase {
	public:
		using AttributesBuffer = std::ostringstream;

		/**
		 * Constructor
		 * 
		 * @param attributes The attributes
		 */
		template <class ...Attributes>
		AttributedPrimitiveBase(Attributes &&...attributes) {
			addAttributes(attributes...);
		}

		/**
		 * Copy constructor. We need to define a copy constructor 
		 * since stringstraem is not copyable by default
		 */
		AttributedPrimitiveBase(const AttributedPrimitiveBase & rhs)
			: attributesBuffer(rhs.attributesBuffer.str()) {}

		virtual void stream(std::ostream & os) const {};

		/**
		* Add attribtues to the attribute primitive, it will be
		* searlized and save to the attributesBuffer
		*
		* @param placeholder Not use, just make the attributes work
		* @param attributes The attributes to add
		*/
		template <class ...Attributes>
		void addAttributes(Attributes &&...attributes) {
			VPly::writeVPlyAttribute(attributesBuffer, attributes...);
		}

	protected:
		AttributesBuffer attributesBuffer;
	};

	/**
	 * A class representation for a single point with attribute, which is used as a 
	 * basic building block for attributed primitives
	 */
	class AttributedPoint: public AttributedPrimitiveBase {
	public:

		/**
		 * Constructor
		 * 
		 * @param pos The position
		 * @param attributes Additional attributes
		 */
		template<class ...Attributes>
		AttributedPoint(const VPlyVector3f & pos, Attributes &&...attributes) :
			AttributedPrimitiveBase(attributes...), pos(pos) {}

		AttributedPoint(const AttributedPoint &) = default;

		/**
		 * Searlize the AttributedPoint to a stream, which is the same as writePoint function
		 * 
		 * @os The output stream
		 */
		void stream(std::ostream & os) const override {
			writePrimitiveID(os, PrimitiveID::Point);
			pos.stream(os);
			os << attributesBuffer.str(); // We copy the buffer stream directly to the output stream
		}

	private:
		VPlyVector3f pos; ///< Position
	};

	/**
	 * Representation of a line with attributed in each point. A AttributedLineStrip is a 
	 * line strip which is composed several AttributedPoint. It itself can have several attributes.
	 */
	class AttributedLineStrip: public AttributedPrimitiveBase {
	public:

		/**
		 * Constructor
		 * 
		 * @param order The order of the line strip
		 * @param attributes Additional attributes
		 */
		template <class ...Attributes>
		AttributedLineStrip(int32_t order, Attributes &&...attributes):
			AttributedPrimitiveBase(attributes...), order(order) {}

		void addPoint(const AttributedPoint & point) {
			points.push_back(point);
		}

		template <class ...Attributes>
		void addPoint(const VPlyVector3f & pos, Attributes &&...attributes) {
			points.emplace_back(pos, attributes...);
		}

		void stream(std::ostream& os) const override {
			writePrimitiveID(os, PrimitiveID::AttributedLineStrip_);
			os.write((char*)(&order), sizeof(int32_t)); // Write order

			int32_t npoint = static_cast<int32_t>(points.size());
			os.write((char*)(&npoint), sizeof(int32_t));

			for (const auto & point : points)
				point.stream(os);

			os << attributesBuffer.str();
		}

	private:
		int32_t order; ///< The order of the line strip, order=2 means a group of line segements
		std::vector<AttributedPoint> points; ///< The points that defines the line strip
	};
}
