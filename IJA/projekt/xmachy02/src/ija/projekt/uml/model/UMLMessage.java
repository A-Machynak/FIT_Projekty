package ija.projekt.uml.model;

/**
 * Represents message sent between two objects - sender and receiver
 */
public class UMLMessage extends UMLElement {
    public enum UMLMessageType {
        ASYNC_MESSAGE,
        SYNC_MESSAGE,
        RETURN_MESSAGE
    };

    private UMLLifeline sender;
    private UMLLifeline receiver;
    private UMLOperation operation;

    public UMLMessage(UMLLifeline sender, UMLLifeline receiver, UMLOperation operation) {
        super("Message");
        this.sender = sender;
        this.receiver = receiver;
        this.operation = operation;
    }

    public UMLLifeline getSender() {
        return sender;
    }

    public void setSender(UMLLifeline sender) {
        this.sender = sender;
    }

    public UMLLifeline getReceiver() {
        return receiver;
    }

    public void setReceiver(UMLLifeline receiver) {
        this.receiver = receiver;
    }

    public UMLOperation getOperation() {
        return operation;
    }

    public void setOperation(UMLOperation operation) {
        this.operation = operation;
    }
}
