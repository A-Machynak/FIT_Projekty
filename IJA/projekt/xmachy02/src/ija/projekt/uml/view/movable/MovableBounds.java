package ija.projekt.uml.view.movable;

/**
 * MovableBounds allows classes implementing this interface to specify their own "virtual size"
 */
public interface MovableBounds {
    int getTop();
    int getBottom();
    int getLeft();
    int getRight();
}
