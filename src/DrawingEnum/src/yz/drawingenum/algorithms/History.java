package yz.drawingenum.algorithms;

import java.util.ArrayList;

public class History {

  private class AddFreeNodeItem {
    final Node node;
    final String region;

    public AddFreeNodeItem(Node node, String region) {
      this.node = node;
      this.region = region;
    }

    public String toString() {
      return "Add free node " + node + " to region " + region;
    }
  }

  private class AddChordItem {
    final Node node1, node2;
    final String region;

    public AddChordItem(Node node1, Node node2, String region) {
      this.node1 = node1;
      this.node2 = node2;
      this.region = region;
    }

    public String toString() {
      return "Add chord between " + node1 + " and " + node2 + " in region " + region;
    }
  }

  private class AddCrossingItem {
    final Node nodeFrom, nodeTo;
    final Arc throughArc;

    public AddCrossingItem(Node nodeFrom, Node nodeTo, Arc throughArc) {
      this.nodeFrom = nodeFrom;
      this.nodeTo = nodeTo;
      this.throughArc = throughArc;
    }

    public String toString() {
      return "Add crossing from " + nodeFrom + " through " + throughArc + "; new node is " + nodeTo;
    }
  }

  final ArrayList<Object> historyItems;

  History() {
    historyItems = new ArrayList<Object>();
  }

  History(History h) {
    historyItems = (ArrayList<Object>) h.historyItems.clone();
  }

  public void addFreeNode(Node node, String region) {
    historyItems.add(new AddFreeNodeItem(node, region));
  }

  public void addChord(Node node1, Node node2, String region) {
    historyItems.add(new AddChordItem(node1, node2, region));
  }

  public void addCrossing(Node nodeFrom, Node nodeTo, Arc throughArc) {
    historyItems.add(new AddCrossingItem(nodeFrom, nodeTo, throughArc));
  }

  public void removeLast() {
    historyItems.remove(historyItems.size() - 1);
  }

  public String[] toArray() {
    String[] array = new String[historyItems.size()];
    int i = 0;
    for (Object o : historyItems) {
      array[i++] = o.toString();
    }
    return array;
  }

}
