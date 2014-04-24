package yz.drawingenum.algorithms;

import yz.drawingenum.algorithms.Node.NodeType;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;

import static yz.utils.MoreMath.mod;

class Region {
  private final ArrayList<Node> boundary;
  private final HashMap<Node, Node> leafNodes;
  private final HashSet<Node> freeNodes;
  public Orientation orientation = Orientation.UNDEFINED;

  public enum Orientation {
    UNDEFINED, CLOCKWISE, COUNTERCLOCKWISE
  }

  public String toString() {
    String bdd = "";
    for (Node n : boundary) {
      bdd += " " + n;
    }
    bdd = "(" + bdd.trim() + ")";
    switch (orientation) {
      case CLOCKWISE:
        bdd += "+";
        break;
      case COUNTERCLOCKWISE:
        bdd += "-";
        break;
      case UNDEFINED:
        bdd += "?";
        break;
    }

    String lf = "";
    for (Node n : leafNodes.keySet()) {
      lf += "(" + n + "," + leafNodes.get(n) + ") ";
    }
    lf = "{" + lf.trim() + "}";

    String f = "";
    for (Node n : freeNodes) {
      f += n + " ";
    }
    f = "{" + f.trim() + "}";

    return "(" + bdd + "," + lf + "," + f + ")";
  }

  public Region() {
    boundary = new ArrayList<Node>();
    leafNodes = new HashMap<Node, Node>();
    freeNodes = new HashSet<Node>();
  }

  @SuppressWarnings("unchecked")
  public Region(Region r2) {
    boundary = new ArrayList<Node>(r2.boundary);
    leafNodes = (HashMap<Node, Node>) r2.leafNodes.clone();
    freeNodes = (HashSet<Node>) r2.freeNodes.clone();

    orientation = r2.orientation;
  }

  public Arc[] getBoundaryArcs() {
    Arc[] arcs = new Arc[boundary.size()];
    int bdSize = boundary.size();
    for (int i = 0; i < bdSize; i++) {
      arcs[i] = new Arc(boundary.get(i), boundary.get((i + 1) % bdSize));
    }
    return arcs;
  }

  public boolean boundaryContainsArc(Arc arc) {
    int n = boundary.size();

    for (int i = 0; i < n; i++) {
      if ((boundary.get(i) == arc.u) && (boundary.get((i + 1) % n) == arc.v)) {
        return true;
      }
      if ((boundary.get(i) == arc.v) && (boundary.get((i + 1) % n) == arc.u)) {
        return true;
      }
    }

    return false;
  }

  public Collection<Node> getBoundaryNodes() {
    return boundary;
  }

  public Collection<Node> getLeafNodes() {
    return leafNodes.keySet();
  }

  public Collection<Node> getFreeNodes() {
    return freeNodes;
  }

  public void subdivideBoundary(Arc arc, Node uv) {
    subdivideBoundary(arc.u, arc.v, uv);
  }

  public void subdivideBoundary(Node u, Node v, Node uv) {

    int n = boundary.size();
    int ui = -1, vi = -1;

    for (int i = 0; i < n; i++) {
      if ((boundary.get(i) == u) && (boundary.get((i + 1) % n) == v)) {
        ui = i;
        vi = (i + 1) % n;
        break;
      }
      if ((boundary.get(i) == v) && (boundary.get((i + 1) % n) == u)) {
        vi = i;
        ui = (i + 1) % n;
        break;
      }
    }
    assert (ui >= 0);
    assert (vi >= 0);

    int min = (ui > vi) ? vi : ui, max = (ui > vi) ? ui : vi;
    int bdSize = boundary.size();

    assert (min != max);

    if ((min == 0) && (max == bdSize - 1)) {
      boundary.add(bdSize, uv);
    } else {
      assert (min == max - 1);
      boundary.add(min + 1, uv);
    }
  }

  public Region[] split(Node a, Node b) {
    return split(a, b, null);
  }

  public Region[] split(Node a, Node b, Node nodeInbetween) {
    if (freeNodes.size() > 0) {
      throw new java.lang.Error("Cannot split a region that contains free nodes");
    }

    // split region R into two parts
    Region R1 = new Region();
    Region R2 = new Region();

    R1.orientation = orientation;
    R2.orientation = orientation;

    // find a and b on the boundary
    int stage = 0;
    int bdsize = boundary.size();
    for (int i = 0; (i < 2 * bdsize) && (stage <= 2); i++) {
      Node ni = boundary.get(i % bdsize);
      if ((ni == a) || (ni == b)) {
        if (stage == 1) {
          R1.boundary.add(ni);
        } else if (stage == 2) {
          R2.boundary.add(ni);
        }
        stage++;
      }
      if (stage == 1) {
        R1.boundary.add(ni);
      } else if (stage == 2) {
        R2.boundary.add(ni);
      }
    }
    if (nodeInbetween != null) {
      R1.boundary.add(nodeInbetween);
      R2.boundary.add(nodeInbetween);
    }

    // put leaf vertices in the correct region
    for (Node leafNode : leafNodes.keySet()) {
      Node parent = getLeafParent(leafNode);
      if ((parent == a) || (parent == b)) {
        throw new java.lang.Error("Trying to split region " + this + " on nodes " + a + " and " + b + ", but " + parent
            + " has leaf node " + leafNode + ".");
      }
      if (R1.contains(parent, NodeType.Boundary)) {
        R1.addLeafNode(leafNode, parent);
      }
      if (R2.contains(parent, NodeType.Boundary)) {
        R2.addLeafNode(leafNode, parent);
      }
    }
    return new Region[]{R1, R2};
  }

  public boolean contains(Node z, NodeType t) {
    switch (t) {
      case Boundary:
        return boundary.contains(z);
      case Free:
        return freeNodes.contains(z);
      case Leaf:
        return leafNodes.containsKey(z);
    }
    return false;
  }

  public Node getLeafParent(Node a) {
    return leafNodes.get(a);
  }

  public void addFreeNode(Node a) {
    freeNodes.add(a);
  }

  public Node getBoundaryNode(int index) {
    return boundary.get(index);
  }

  public void addBoundaryNode(Node a) {
    boundary.add(a);
  }

  public void removeFreeNode(Node a) {
    assert (freeNodes.contains(a));
    freeNodes.remove(a);
  }

  public void removeLeafNode(Node a) {
    assert (leafNodes.containsKey(a));
    leafNodes.remove(a);
  }

  public void addLeafNode(Node a, Node parent) {
    leafNodes.put(a, parent);
  }

  /**
   * getClockwiseEarliestNeighbor chooses the neighbors x1 and x2 of u such
   * that x1 < z < x2 in the clockwise order for any vertex z inside the
   * region, and returns x1. Clearly, this depends on the orientation of the
   * region.
   */
  public Node getClockwiseEarliestNeighbor(Node u) {
    assert (orientation != Orientation.UNDEFINED);

    int index = boundary.indexOf(u);
    switch (orientation) {
      case CLOCKWISE:
        return boundary.get(mod(index + 1, boundary.size()));
      case COUNTERCLOCKWISE:
        return boundary.get(mod(index - 1, boundary.size()));
    }

    return null;
  }

}
