/**
 * @file Pair.java
 * @author Augustin Machynak (xmachy02@stud.fit.vutbr.cz), FIT 2BIT
 * @brief Generic pair
 *
 */

package ija.projekt.uml.utils;

public class Pair<K, V> {
    private K first;
    private V second;

    public Pair() { }

    public Pair(K first, V second) {
        this.first = first;
        this.second = second;
    }

    public K getFirst() {
        return first;
    }

    public void setFirst(K first) {
        this.first = first;
    }

    public V getSecond() {
        return second;
    }

    public void setSecond(V second) {
        this.second = second;
    }

    @Override
    public String toString() {
        return "Pair{" +
                "first=" + first +
                ", second=" + second +
                '}';
    }
}
