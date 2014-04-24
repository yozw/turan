package yz.drawingenum.algorithms;

import java.util.ArrayList;

public class EdgeList extends ArrayList<Edge> {

  public Edge getEdge(Vertex u, Vertex v) {
    for (Edge e : this) {
      if (e.hasEndpoints(u, v)) {
        return e;
      }
    }
    return null;
  }
}
