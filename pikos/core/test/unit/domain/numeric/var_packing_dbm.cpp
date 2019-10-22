/*******************************************************************************
 *
 * Tests for VarPackingDBM
 *
 * Author: Maxime Arthaud
 *
 * Contact: ikos@lists.nasa.gov
 *
 * Notices:
 *
 * Copyright (c) 2018-2019 United States Government as represented by the
 * Administrator of the National Aeronautics and Space Administration.
 * All Rights Reserved.
 *
 * Disclaimers:
 *
 * No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF
 * ANY KIND, EITHER EXPRESSED, IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED
 * TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS,
 * ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * OR FREEDOM FROM INFRINGEMENT, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE
 * ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED, WILL CONFORM TO
 * THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN
 * ENDORSEMENT BY GOVERNMENT AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS,
 * RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 * RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY
 * DISCLAIMS ALL WARRANTIES AND LIABILITIES REGARDING THIRD-PARTY SOFTWARE,
 * IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:  RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST
 * THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL
 * AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE RESULTS
 * IN ANY LIABILITIES, DEMANDS, DAMAGES, EXPENSES OR LOSSES ARISING FROM SUCH
 * USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 * RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD
 * HARMLESS THE UNITED STATES GOVERNMENT, ITS CONTRACTORS AND SUBCONTRACTORS,
 * AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 * RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE,
 * UNILATERAL TERMINATION OF THIS AGREEMENT.
 *
 ******************************************************************************/

#define BOOST_TEST_MODULE test_var_packing_dbm
#define BOOST_TEST_DYN_LINK
#include <boost/mpl/list.hpp>
#include <boost/test/output_test_stream.hpp>
#include <boost/test/unit_test.hpp>

#include <ikos/core/domain/numeric/var_packing_dbm.hpp>
#include <ikos/core/example/variable_factory.hpp>

using ZNumber = ikos::core::ZNumber;
using VariableFactory = ikos::core::example::VariableFactory;
using Variable = ikos::core::example::VariableFactory::VariableRef;
using VariableExpr = ikos::core::VariableExpression< ZNumber, Variable >;
using BinaryOperator = ikos::core::numeric::BinaryOperator;
using Bound = ikos::core::ZBound;
using Interval = ikos::core::numeric::ZInterval;
using Congruence = ikos::core::numeric::ZCongruence;
using IntervalCongruence = ikos::core::numeric::IntervalCongruence< ZNumber >;
using VarPackingDBM = ikos::core::numeric::VarPackingDBM< ZNumber, Variable >;

BOOST_AUTO_TEST_CASE(is_top_and_bottom) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));

  BOOST_CHECK(VarPackingDBM::top().is_top());
  BOOST_CHECK(!VarPackingDBM::top().is_bottom());

  BOOST_CHECK(!VarPackingDBM::bottom().is_top());
  BOOST_CHECK(VarPackingDBM::bottom().is_bottom());

  VarPackingDBM inv;
  BOOST_CHECK(inv.is_top());
  BOOST_CHECK(!inv.is_bottom());

  inv.set(x, Interval(1));
  BOOST_CHECK(!inv.is_top());
  BOOST_CHECK(!inv.is_bottom());

  inv.set(x, Interval::bottom());
  BOOST_CHECK(!inv.is_top());
  BOOST_CHECK(inv.is_bottom());

  inv.set_to_top();
  inv.add(VariableExpr(x) - VariableExpr(y) <= 1);
  inv.forget(x);
  BOOST_CHECK(inv.is_top());
}

BOOST_AUTO_TEST_CASE(set_to_top_and_bottom) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));

  VarPackingDBM inv;
  BOOST_CHECK(inv.is_top());
  BOOST_CHECK(!inv.is_bottom());

  inv.set_to_bottom();
  BOOST_CHECK(!inv.is_top());
  BOOST_CHECK(inv.is_bottom());

  inv.set_to_top();
  BOOST_CHECK(inv.is_top());
  BOOST_CHECK(!inv.is_bottom());
}

BOOST_AUTO_TEST_CASE(leq) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable a(vfac.get("a"));
  Variable b(vfac.get("b"));
  Variable c(vfac.get("c"));

  BOOST_CHECK(VarPackingDBM::bottom().leq(VarPackingDBM::top()));
  BOOST_CHECK(VarPackingDBM::bottom().leq(VarPackingDBM::bottom()));
  BOOST_CHECK(!VarPackingDBM::top().leq(VarPackingDBM::bottom()));
  BOOST_CHECK(VarPackingDBM::top().leq(VarPackingDBM::top()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(0));
  BOOST_CHECK(inv1.leq(VarPackingDBM::top()));
  BOOST_CHECK(!inv1.leq(VarPackingDBM::bottom()));

  VarPackingDBM inv2;
  inv2.set(x, Interval(Bound(-1), Bound(1)));
  BOOST_CHECK(inv2.leq(VarPackingDBM::top()));
  BOOST_CHECK(!inv2.leq(VarPackingDBM::bottom()));
  BOOST_CHECK(inv1.leq(inv2));
  BOOST_CHECK(!inv2.leq(inv1));

  VarPackingDBM inv3;
  inv3.set(x, Interval(0));
  inv3.set(y, Interval(Bound(-1), Bound(1)));
  BOOST_CHECK(inv3.leq(VarPackingDBM::top()));
  BOOST_CHECK(!inv3.leq(VarPackingDBM::bottom()));
  BOOST_CHECK(inv3.leq(inv1));
  BOOST_CHECK(!inv1.leq(inv3));

  VarPackingDBM inv4;
  inv4.set(x, Interval(0));
  inv4.set(y, Interval(Bound(0), Bound(2)));
  BOOST_CHECK(inv4.leq(VarPackingDBM::top()));
  BOOST_CHECK(!inv4.leq(VarPackingDBM::bottom()));
  BOOST_CHECK(!inv3.leq(inv4));
  BOOST_CHECK(!inv4.leq(inv3));

  VarPackingDBM inv5;
  inv5.set(x, Interval(0));
  inv5.set(y, Interval(Bound(0), Bound(2)));
  inv5.set(z, Interval(Bound::minus_infinity(), Bound(0)));
  BOOST_CHECK(inv5.leq(VarPackingDBM::top()));
  BOOST_CHECK(!inv5.leq(VarPackingDBM::bottom()));
  BOOST_CHECK(!inv5.leq(inv3));
  BOOST_CHECK(!inv3.leq(inv5));
  BOOST_CHECK(inv5.leq(inv4));
  BOOST_CHECK(!inv4.leq(inv5));

  inv1.set_to_top();
  inv2.set_to_top();
  inv1.assign(x, 1);
  BOOST_CHECK(inv1.leq(inv2));

  inv2.add(VariableExpr(x) <= 1);
  BOOST_CHECK(inv1.leq(inv2)); // {x = 1} <= {x <= 1}

  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 0);
  BOOST_CHECK(!inv1.leq(inv2)); // not {x = 1} <= {x <= 0}

  inv1.assign(y, 2);
  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 1);
  BOOST_CHECK(inv1.leq(inv2)); // {x = 1, y = 2} <= {x <= 1}

  inv2.add(VariableExpr(z) <= 4);
  BOOST_CHECK(!inv1.leq(inv2)); // not {x = 1, y = 2} <= {x <= 1, z <= 4}

  inv1.set_to_top();
  inv2.set_to_top();

  inv1.assign(x, 1);
  inv1.add(VariableExpr(y) <= 2);
  inv1.assign(z, 3);
  inv1.add(VariableExpr(a) >= 4);
  inv1.assign(b, 5);

  inv2.add(VariableExpr(y) <= 3);
  inv2.add(VariableExpr(a) >= 1);
  inv2.assign(z, 3);
  inv2.set(x, Interval(Bound(-1), Bound(1)));

  // {x = 1, y <= 2, z = 3, a >= 4, b = 5} <= {-1 <= x <= 1, y <= 3, z = 3, a >=
  // 1}
  BOOST_CHECK(inv1.leq(inv2));

  inv2.add(VariableExpr(a) >= 5);
  // {x = 1, y <= 2, z = 3, a >= 4, b = 5} <= {-1 <= x <= 1, y <= 3, z = 3, a >=
  // 5}
  BOOST_CHECK(!inv1.leq(inv2));
}

BOOST_AUTO_TEST_CASE(equals) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  BOOST_CHECK(!VarPackingDBM::bottom().equals(VarPackingDBM::top()));
  BOOST_CHECK(VarPackingDBM::bottom().equals(VarPackingDBM::bottom()));
  BOOST_CHECK(!VarPackingDBM::top().equals(VarPackingDBM::bottom()));
  BOOST_CHECK(VarPackingDBM::top().equals(VarPackingDBM::top()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(0));
  BOOST_CHECK(!inv1.equals(VarPackingDBM::top()));
  BOOST_CHECK(!inv1.equals(VarPackingDBM::bottom()));
  BOOST_CHECK(inv1.equals(inv1));

  VarPackingDBM inv2;
  inv2.set(x, Interval(Bound(-1), Bound(1)));
  BOOST_CHECK(!inv2.equals(VarPackingDBM::top()));
  BOOST_CHECK(!inv2.equals(VarPackingDBM::bottom()));
  BOOST_CHECK(!inv1.equals(inv2));
  BOOST_CHECK(!inv2.equals(inv1));

  VarPackingDBM inv3;
  inv3.set(x, Interval(0));
  inv3.set(y, Interval(Bound(-1), Bound(1)));
  BOOST_CHECK(!inv3.equals(VarPackingDBM::top()));
  BOOST_CHECK(!inv3.equals(VarPackingDBM::bottom()));
  BOOST_CHECK(!inv3.equals(inv1));
  BOOST_CHECK(!inv1.equals(inv3));
}

BOOST_AUTO_TEST_CASE(join) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable a(vfac.get("a"));
  Variable b(vfac.get("b"));
  Variable c(vfac.get("c"));

  BOOST_CHECK((VarPackingDBM::bottom().join(VarPackingDBM::top()) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::bottom().join(VarPackingDBM::bottom()) ==
               VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().join(VarPackingDBM::top()) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::top().join(VarPackingDBM::bottom()) ==
               VarPackingDBM::top()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(Bound(0), Bound(1)));
  BOOST_CHECK((inv1.join(VarPackingDBM::top()) == VarPackingDBM::top()));
  BOOST_CHECK((inv1.join(VarPackingDBM::bottom()) == inv1));
  BOOST_CHECK((VarPackingDBM::top().join(inv1) == VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::bottom().join(inv1) == inv1));
  BOOST_CHECK((inv1.join(inv1) == inv1));

  VarPackingDBM inv2, inv3;
  inv2.set(x, Interval(Bound(-1), Bound(0)));
  inv3.set(x, Interval(Bound(-1), Bound(1)));
  BOOST_CHECK((inv1.join(inv2) == inv3));
  BOOST_CHECK((inv2.join(inv1) == inv3));

  VarPackingDBM inv4;
  inv4.set(x, Interval(Bound(-1), Bound(0)));
  inv4.set(y, Interval(0));
  BOOST_CHECK((inv4.join(inv2) == inv2));
  BOOST_CHECK((inv2.join(inv4) == inv2));

  inv1.set_to_top();
  inv1.assign(x, 1);

  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 1);

  BOOST_CHECK((inv1.join(inv2) == inv2)); // {x = 1} U {x <= 1}

  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 0);

  inv3.set_to_top();
  inv3.add(VariableExpr(x) <= 1);

  BOOST_CHECK((inv1.join(inv2) == inv3)); // {x = 1} U {x <= 0}

  inv1.assign(y, 2);

  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 1);
  BOOST_CHECK((inv1.join(inv2) == inv2)); // {x = 1, y = 2} U {x <= 1}

  inv2.add(VariableExpr(z) <= 4);

  inv3.set_to_top();
  inv3.add(VariableExpr(x) <= 1);

  BOOST_CHECK((inv1.join(inv2) == inv3)); // {x = 1, y = 2} U {x <= 1, z <= 4}

  inv1.set_to_top();
  inv1.assign(x, 1);
  inv1.add(VariableExpr(y) <= 2);
  inv1.assign(z, 3);
  inv1.add(VariableExpr(a) >= 4);
  inv1.assign(b, 5);

  inv2.set_to_top();
  inv2.add(VariableExpr(y) <= 3);
  inv2.add(VariableExpr(a) >= 1);
  inv2.assign(z, 3);
  inv2.set(x, Interval(Bound(-1), Bound(1)));

  inv3.set_to_top();
  inv3.set(x, Interval(Bound(-1), Bound(1)));
  inv3.add(VariableExpr(y) <= 3);
  inv3.assign(z, 3);
  inv3.add(VariableExpr(a) >= 1);

  // {x = 1, y <= 2, z = 3, a >= 4, b = 5} U {-1 <= x <= 1, y <= 3, z = 3, a >=
  // 1}
  BOOST_CHECK((inv1.join(inv2) == inv3));

  inv2.add(VariableExpr(a) >= 5);

  // {x = 1, y <= 2, z = 3, a >= 4, b = 5} U {-1 <= x <= 1, y <= 3, z = 3, a >=
  // 5}
  BOOST_CHECK((inv1.join(inv2).to_interval(a) ==
               Interval(Bound(4), Bound::plus_infinity())));

  inv1.set_to_top();
  inv1.assign(x, 1);
  inv1.add(VariableExpr(y) <= 10);
  inv1.add((VariableExpr(z) - VariableExpr(y)) <= 10);

  inv2.set_to_top();
  inv2.assign(y, 0);
  inv2.assign(z, 0);

  inv3.set_to_top();
  inv3.add(VariableExpr(y) <= 10);
  inv3.add((VariableExpr(z) - VariableExpr(y)) <= 10);

  // {x = 1, y <= 10, z - y <= 10} U {y = 0, z = 0}
  BOOST_CHECK((inv1.join(inv2) == inv3));
}

BOOST_AUTO_TEST_CASE(widening) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  BOOST_CHECK((VarPackingDBM::bottom().widening(VarPackingDBM::top()) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::bottom().widening(VarPackingDBM::bottom()) ==
               VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().widening(VarPackingDBM::top()) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::top().widening(VarPackingDBM::bottom()) ==
               VarPackingDBM::top()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(Bound(0), Bound(1)));
  BOOST_CHECK((inv1.widening(VarPackingDBM::top()) == VarPackingDBM::top()));
  BOOST_CHECK((inv1.widening(VarPackingDBM::bottom()) == inv1));
  BOOST_CHECK((VarPackingDBM::top().widening(inv1) == VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::bottom().widening(inv1) == inv1));
  BOOST_CHECK((inv1.widening(inv1) == inv1));

  VarPackingDBM inv2, inv3;
  inv2.set(x, Interval(Bound(0), Bound(2)));
  inv3.set(x, Interval(Bound(0), Bound::plus_infinity()));
  BOOST_CHECK((inv1.widening(inv2) == inv3));
  BOOST_CHECK((inv2.widening(inv1) == inv2));
}

BOOST_AUTO_TEST_CASE(widening_threshold) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  BOOST_CHECK((VarPackingDBM::bottom().widening_threshold(VarPackingDBM::top(),
                                                          ZNumber(10)) ==
               VarPackingDBM::top()));
  BOOST_CHECK(
      (VarPackingDBM::bottom().widening_threshold(VarPackingDBM::bottom(),
                                                  ZNumber(10)) ==
       VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().widening_threshold(VarPackingDBM::top(),
                                                       ZNumber(10)) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::top().widening_threshold(VarPackingDBM::bottom(),
                                                       ZNumber(10)) ==
               VarPackingDBM::top()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(Bound(0), Bound(1)));
  BOOST_CHECK((inv1.widening_threshold(VarPackingDBM::top(), ZNumber(10)) ==
               VarPackingDBM::top()));
  BOOST_CHECK(
      (inv1.widening_threshold(VarPackingDBM::bottom(), ZNumber(10)) == inv1));
  BOOST_CHECK((VarPackingDBM::top().widening_threshold(inv1, ZNumber(10)) ==
               VarPackingDBM::top()));
  BOOST_CHECK(
      (VarPackingDBM::bottom().widening_threshold(inv1, ZNumber(10)) == inv1));
  BOOST_CHECK((inv1.widening_threshold(inv1, ZNumber(10)) == inv1));

  VarPackingDBM inv2, inv3;
  inv2.set(x, Interval(Bound(0), Bound(2)));
  inv3.set(x, Interval(Bound(0), Bound(10)));
  BOOST_CHECK((inv1.widening_threshold(inv2, ZNumber(10)) == inv3));
  BOOST_CHECK((inv2.widening_threshold(inv1, ZNumber(10)) == inv2));
}

BOOST_AUTO_TEST_CASE(meet) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable a(vfac.get("a"));
  Variable b(vfac.get("b"));
  Variable c(vfac.get("c"));

  BOOST_CHECK((VarPackingDBM::bottom().meet(VarPackingDBM::top()) ==
               VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::bottom().meet(VarPackingDBM::bottom()) ==
               VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().meet(VarPackingDBM::top()) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::top().meet(VarPackingDBM::bottom()) ==
               VarPackingDBM::bottom()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(Bound(0), Bound(1)));
  BOOST_CHECK((inv1.meet(VarPackingDBM::top()) == inv1));
  BOOST_CHECK((inv1.meet(VarPackingDBM::bottom()) == VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().meet(inv1) == inv1));
  BOOST_CHECK((VarPackingDBM::bottom().meet(inv1) == VarPackingDBM::bottom()));
  BOOST_CHECK((inv1.meet(inv1) == inv1));

  VarPackingDBM inv2, inv3;
  inv2.set(x, Interval(Bound(-1), Bound(0)));
  inv3.set(x, Interval(0));
  BOOST_CHECK((inv1.meet(inv2) == inv3));
  BOOST_CHECK((inv2.meet(inv1) == inv3));

  VarPackingDBM inv4, inv5;
  inv4.set(x, Interval(Bound(0), Bound(1)));
  inv4.set(y, Interval(0));
  inv5.set(x, Interval(0));
  inv5.set(y, Interval(0));
  BOOST_CHECK((inv4.meet(inv2) == inv5));
  BOOST_CHECK((inv2.meet(inv4) == inv5));

  inv1.set_to_top();
  inv1.assign(x, 1);

  inv2.set_to_top();

  BOOST_CHECK((inv1.meet(inv2) == inv1)); // {x = 1} & top()

  inv2.add(VariableExpr(x) <= 1);
  BOOST_CHECK((inv1.meet(inv2) == inv1)); // {x = 1} & {x <= 1}

  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 0);
  BOOST_CHECK(
      (inv1.meet(inv2) == VarPackingDBM::bottom())); // {x = 1} & {x <= 0}

  inv1.assign(y, 2);

  inv2.set_to_top();
  inv2.add(VariableExpr(x) <= 1);
  BOOST_CHECK((inv1.meet(inv2) == inv1)); // {x = 1, y = 2} & {x <= 1}

  inv2.add(VariableExpr(z) <= 4);

  inv3.set_to_top();
  inv3.assign(x, 1);
  inv3.assign(y, 2);
  inv3.add(VariableExpr(z) <= 4);
  BOOST_CHECK((inv1.meet(inv2) == inv3)); // {x = 1, y = 2} & {x <= 1, z <= 4}

  inv1.set_to_top();
  inv1.assign(x, 1);
  inv1.add(VariableExpr(y) <= 2);
  inv1.assign(z, 3);
  inv1.add(VariableExpr(a) >= 4);
  inv1.assign(b, 5);

  inv2.set_to_top();
  inv2.add(VariableExpr(y) <= 3);
  inv2.add(VariableExpr(a) >= 1);
  inv2.assign(z, 3);
  inv2.set(x, Interval(Bound(-1), Bound(1)));

  inv3.set_to_top();
  inv3.assign(x, 1);
  inv3.add(VariableExpr(y) <= 2);
  inv3.assign(z, 3);
  inv3.add(VariableExpr(a) >= 4);
  inv3.assign(b, 5);

  // {x = 1, y <= 2, z = 3, a >= 4, b = 5} & {-1 <= x <= 1, y <= 3, z = 3, a >=
  // 1}
  BOOST_CHECK((inv1.meet(inv2) == inv3));

  inv2.add(VariableExpr(a) >= 5);
  inv3.add(VariableExpr(a) >= 5);

  // {x = 1, y <= 2, z = 3, a >= 4, b = 5} & {-1 <= x <= 1, y <= 3, z = 3, a >=
  // 5}
  BOOST_CHECK((inv1.meet(inv2) == inv3));
}

BOOST_AUTO_TEST_CASE(narrowing) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  BOOST_CHECK((VarPackingDBM::bottom().narrowing(VarPackingDBM::top()) ==
               VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::bottom().narrowing(VarPackingDBM::bottom()) ==
               VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().narrowing(VarPackingDBM::top()) ==
               VarPackingDBM::top()));
  BOOST_CHECK((VarPackingDBM::top().narrowing(VarPackingDBM::bottom()) ==
               VarPackingDBM::bottom()));

  VarPackingDBM inv1;
  inv1.set(x, Interval(Bound(0), Bound::plus_infinity()));
  BOOST_CHECK((inv1.narrowing(VarPackingDBM::top()) == inv1));
  BOOST_CHECK(
      (inv1.narrowing(VarPackingDBM::bottom()) == VarPackingDBM::bottom()));
  BOOST_CHECK((VarPackingDBM::top().narrowing(inv1) == inv1));
  BOOST_CHECK(
      (VarPackingDBM::bottom().narrowing(inv1) == VarPackingDBM::bottom()));
  BOOST_CHECK((inv1.narrowing(inv1) == inv1));

  VarPackingDBM inv2, inv3;
  inv2.set(x, Interval(Bound(0), Bound(1)));
  BOOST_CHECK((inv1.narrowing(inv2) == inv2));
  BOOST_CHECK((inv2.narrowing(inv1) == inv2));
}

BOOST_AUTO_TEST_CASE(assign) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv1, inv2;
  inv1.assign(x, 0);
  inv2.set(x, Interval(0));
  BOOST_CHECK((inv1 == inv2));

  inv1.set_to_bottom();
  inv1.assign(x, 0);
  BOOST_CHECK(inv1.is_bottom());

  inv1.set_to_top();
  inv1.set(x, Interval(Bound(-1), Bound(1)));
  inv1.assign(y, x);
  inv1.normalize();
  BOOST_CHECK(inv1.to_interval(y) == Interval(Bound(-1), Bound(1)));

  inv1.set_to_top();
  inv1.set(x, Interval(Bound(-1), Bound(1)));
  inv1.set(y, Interval(Bound(1), Bound(2)));
  inv1.assign(z, 2 * VariableExpr(x) - 3 * VariableExpr(y) + 1);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-7), Bound(0)));

  inv1.set_to_top();
  inv1.assign(x, 7);
  inv1.add(VariableExpr(y) <= 3);
  inv1.add(VariableExpr(y) >= 1);
  inv1.assign(z, VariableExpr(x) + 2 * VariableExpr(y) + 1);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(10), Bound(14)));
}

BOOST_AUTO_TEST_CASE(apply) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv1, inv2;
  inv1.set(x, Interval(Bound(-1), Bound(1)));
  inv1.set(y, Interval(Bound(1), Bound(2)));

  inv1.apply(BinaryOperator::Add, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(3)));

  inv1.apply(BinaryOperator::Sub, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-3), Bound(0)));

  inv1.apply(BinaryOperator::Mul, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-2), Bound(2)));

  inv1.apply(BinaryOperator::Div, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-1), Bound(1)));

  inv1.apply(BinaryOperator::Rem, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-1), Bound(1)));

  inv1.apply(BinaryOperator::Mod, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(1)));

  inv1.apply(BinaryOperator::Shl, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-4), Bound(4)));

  inv1.apply(BinaryOperator::Shr, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-1), Bound(0)));

  inv1.apply(BinaryOperator::And, z, x, y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(2)));

  inv1.apply(BinaryOperator::Add, z, x, ZNumber(3));
  inv1.normalize();
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(2), Bound(4)));

  inv1.apply(BinaryOperator::Sub, z, x, ZNumber(3));
  inv1.normalize();
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-4), Bound(-2)));

  inv1.apply(BinaryOperator::Mul, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-3), Bound(3)));

  inv1.apply(BinaryOperator::Div, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(0)));

  inv1.apply(BinaryOperator::Rem, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-1), Bound(1)));

  inv1.apply(BinaryOperator::Mod, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(2)));

  inv1.apply(BinaryOperator::Shl, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-8), Bound(8)));

  inv1.apply(BinaryOperator::Shr, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(-1), Bound(0)));

  inv1.apply(BinaryOperator::And, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(3)));

  inv1.apply(BinaryOperator::Or, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval::top());

  inv1.apply(BinaryOperator::Xor, z, x, ZNumber(3));
  BOOST_CHECK(inv1.to_interval(z) == Interval::top());

  inv1.apply(BinaryOperator::Add, z, ZNumber(4), y);
  inv1.normalize();
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(5), Bound(6)));

  inv1.apply(BinaryOperator::Sub, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(2), Bound(3)));

  inv1.apply(BinaryOperator::Mul, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(4), Bound(8)));

  inv1.apply(BinaryOperator::Div, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(2), Bound(4)));

  inv1.apply(BinaryOperator::Rem, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(1)));

  inv1.apply(BinaryOperator::Mod, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(1)));

  inv1.apply(BinaryOperator::Shl, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(8), Bound(16)));

  inv1.apply(BinaryOperator::Shr, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(1), Bound(2)));

  inv1.apply(BinaryOperator::And, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(2)));

  inv1.apply(BinaryOperator::Or, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(7)));

  inv1.apply(BinaryOperator::Xor, z, ZNumber(4), y);
  BOOST_CHECK(inv1.to_interval(z) == Interval(Bound(0), Bound(7)));
}

BOOST_AUTO_TEST_CASE(add) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.add(VariableExpr(x) >= 1);
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound::plus_infinity()));

  inv.add(VariableExpr(y) >= VariableExpr(x) + 2);
  inv.normalize();
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound::plus_infinity()));
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(3), Bound::plus_infinity()));

  inv.add(2 * VariableExpr(x) + 3 * VariableExpr(y) <= VariableExpr(z));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound::plus_infinity()));
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(3), Bound::plus_infinity()));
  BOOST_CHECK(inv.to_interval(z) ==
              Interval(Bound(11), Bound::plus_infinity()));

  inv.add(2 * VariableExpr(z) <= 4 * VariableExpr(y));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound::plus_infinity()));
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(5), Bound::plus_infinity()));
  BOOST_CHECK(inv.to_interval(z) ==
              Interval(Bound(11), Bound::plus_infinity()));

  inv.add(VariableExpr(z) + VariableExpr(x) <= 20);
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound(9)));
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(5), Bound::plus_infinity()));
  BOOST_CHECK(inv.to_interval(z) == Interval(Bound(11), Bound(19)));

  inv.add(3 * VariableExpr(y) <= VariableExpr(z));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound(9)));
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(5), Bound(6)));
  BOOST_CHECK(inv.to_interval(z) == Interval(Bound(15), Bound(19)));

  inv.add(VariableExpr(x) == VariableExpr(y));
  inv.normalize();
  BOOST_CHECK(inv.is_bottom());

  inv.set_to_top();
  inv.assign(x, 1);
  inv.add(VariableExpr(x) + VariableExpr(y) >= 0);
  inv.add(VariableExpr(x) - VariableExpr(y) >= 3);
  BOOST_CHECK(inv.is_bottom());
}

BOOST_AUTO_TEST_CASE(set) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.set(x, Interval(Bound(1), Bound(2)));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound(2)));

  inv.set(x, Interval::bottom());
  BOOST_CHECK(inv.is_bottom());

  inv.set_to_top();
  inv.set(x, Congruence(1));
  BOOST_CHECK(inv.to_interval(x) == Interval(1));

  inv.set_to_top();
  inv.set(x, Congruence(ZNumber(3), ZNumber(1)));
  BOOST_CHECK(inv.to_interval(x) == Interval::top());

  inv.set_to_top();
  inv.set(x,
          IntervalCongruence(Interval(Bound(1), Bound(4)),
                             Congruence(ZNumber(3), ZNumber(1))));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound(4)));
}

BOOST_AUTO_TEST_CASE(refine) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.refine(x, Interval(Bound(1), Bound(2)));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound(2)));

  inv.refine(x, Interval(Bound(3), Bound(4)));
  BOOST_CHECK(inv.is_bottom());

  inv.set_to_top();
  inv.refine(x, Congruence(1));
  BOOST_CHECK(inv.to_interval(x) == Interval(1));

  inv.set_to_top();
  inv.refine(x, Congruence(ZNumber(3), ZNumber(1)));
  BOOST_CHECK(inv.to_interval(x) == Interval::top());

  inv.set_to_top();
  inv.refine(x, Interval(Bound(2), Bound(9)));
  inv.refine(x, Congruence(ZNumber(3), ZNumber(1)));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(4), Bound(7)));

  inv.set_to_top();
  inv.refine(x, Interval(Bound(2), Bound(9)));
  inv.refine(x,
             IntervalCongruence(Interval(Bound(7), Bound(10)),
                                Congruence(ZNumber(3), ZNumber(1))));
  BOOST_CHECK(inv.to_interval(x) == Interval(7));
}

BOOST_AUTO_TEST_CASE(forget) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.set(x, Interval(Bound(1), Bound(2)));
  inv.set(y, Interval(Bound(3), Bound(4)));
  BOOST_CHECK(inv.to_interval(x) == Interval(Bound(1), Bound(2)));
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(3), Bound(4)));

  inv.forget(x);
  BOOST_CHECK(inv.to_interval(x) == Interval::top());
  BOOST_CHECK(inv.to_interval(y) == Interval(Bound(3), Bound(4)));

  inv.forget(y);
  BOOST_CHECK(inv.is_top());

  inv.set_to_top();
  inv.add(VariableExpr(x) - VariableExpr(y) <= 1);
  inv.forget(x);
  BOOST_CHECK(inv.is_top());
}

BOOST_AUTO_TEST_CASE(to_interval) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.set(x, Interval(Bound(1), Bound(2)));
  inv.set(y, Interval(Bound(3), Bound(4)));
  BOOST_CHECK(inv.to_interval(2 * VariableExpr(x) + 1) ==
              Interval(Bound(3), Bound(5)));
  BOOST_CHECK(inv.to_interval(2 * VariableExpr(x) - 3 * VariableExpr(y) + 1) ==
              Interval(Bound(-9), Bound(-4)));
}

BOOST_AUTO_TEST_CASE(to_congruence) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.set(x, Interval(Bound(1), Bound(2)));
  inv.set(y, Interval(Bound(3), Bound(4)));
  BOOST_CHECK(inv.to_congruence(2 * VariableExpr(x) + 1) ==
              Congruence(ZNumber(2), ZNumber(1)));
  BOOST_CHECK(inv.to_congruence(2 * VariableExpr(x) - 3 * VariableExpr(y) +
                                1) == Congruence::top());
}

BOOST_AUTO_TEST_CASE(to_interval_congruence) {
  VariableFactory vfac;
  Variable x(vfac.get("x"));
  Variable y(vfac.get("y"));
  Variable z(vfac.get("z"));
  Variable w(vfac.get("w"));

  VarPackingDBM inv;
  inv.set(x, Interval(Bound(1), Bound(2)));
  inv.set(y, Interval(Bound(3), Bound(4)));
  BOOST_CHECK(inv.to_interval_congruence(2 * VariableExpr(x) + 1) ==
              IntervalCongruence(Interval(Bound(3), Bound(5)),
                                 Congruence(ZNumber(2), ZNumber(1))));
  BOOST_CHECK(inv.to_interval_congruence(2 * VariableExpr(x) -
                                         3 * VariableExpr(y) + 1) ==
              IntervalCongruence(Interval(Bound(-9), Bound(-4))));
}
