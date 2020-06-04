#ifndef RELVEC_H
#define RELVEC_H


/**
* @file relvec.h
* @author C S Cowden
* @brief Simple relativistic 4-vector.
* @details Declare and define a simple inline
* relativistic 4-vectors.
*/

#include <ostream>
#include <istream>

#include <vector>


namespace cg {

/**
* @brief Relativistic 4-vector
*/
struct relvec {

  /**
  * @brief default constructor.
  */
  relvec() { }

  /**
  * @brief copy constructor
  * @param[in] vec another relvec
  */
  relvec(const relvec& vec):
    t_(vec.t_)
    ,x_(vec.x_)
    ,y_(vec.y_)
    ,z_(vec.z_)
   { }

  /**
  * @brief construct from elements.
  * @details Construct a relvec from components
  * @param[in] t time-like element
  * @param[in] x x-spatial element
  * @param[in] y y-spatial element
  * @param[in] z z-spatial element
  */
  relvec(double t, double x, double y, double z)
    :t_(t)
    ,x_(x)
    ,y_(y)
    ,z_(z)
  { }

  /**
  * @brief construct from time and vector
  * @details Construct a relvec from a time
  * and the first three elements of vec as the spatial 
  * components.
  * @param[in] t time
  * @param[in] vec vector of double or floats representing the
  * spatial components
  */
  template<typename T>
  relvec(double t,const std::vector<T> & vec)
    :t_(t)
  {
    assert(vec.size() >= 3);
    x_ = vec[0];
    y_ = vec[1];
    z_ = vec[2];
  }


  /**
  * @brief construct from vector (length-4)
  * @details Construct a relvec from the first four
  * elements of the vector.  This assumes the 
  * elements are ordered as follows (t,x,y,z).
  * @param[in] vec vector of floating point numbers (double or float).
  */
  template<typename T>
  relvec(const std::vector<T> & vec)
  {
    assert(vec.size() >= 4);
    t_ = vec[0];
    x_ = vec[1];
    y_ = vec[2];
    z_ = vec[3];
  }

  /**
  * @brief destructor
  */
  virtual ~relvec() { }

  /**
  * @brief serialize
  * @param[in] stream The output stream into which to write this vector.
  */
  inline virtual void serialize(std::ostream & stream) const;

  /**
  * @brief deserialize
  * @param[in] stream The intput stream from which to read this node.
  */
  inline virtual void deserialize(std::istream & stream);
  


  /**
  * @brief insertion operator
  * @details This method can be used to insert the vector into a stream.
  */
  inline friend std::ostream& operator<<(std::ostream & stream, const relvec & vec) {
    vec.serialize(stream);
    return stream;
  }
  

  /**
  * @brief extraction operator
  * @details This method can be used to extract the vector from a stream.
  */
  inline friend std::istream& operator>>(std::istream & stream, relvec & vec) {
    vec.deserialize(stream);
    return stream;
  }

  /**
  * @brief equality operator
  * @param[in] vec A vecotr with which to test for equality.
  */
  inline virtual bool operator==(const relvec &);

  /**
  * @brief dot operator
  * @param[in] vec A vector with which to take a dot product.
  * @return a double
  */
  inline virtual double operator*(const relvec &);

  /**
  * @brief addition operator
  * @param[in] vec A vector to add to this one.
  * @return A new relvec
  */
  inline virtual relvec operator+(const relvec &);

  /**
  * @brief subtraction operator
  * @param[in] vec A vector to subtract from this one.
  * @return A new relvec
  */
  inline virtual relvec operator-(const relvec &);


  // ---- public data members ----
  
  double t_;
  double x_;
  double y_;
  double z_;

};



#include "relvec.icc"

}

#endif
