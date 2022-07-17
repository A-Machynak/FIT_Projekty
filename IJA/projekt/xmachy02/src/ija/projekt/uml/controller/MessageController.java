package ija.projekt.uml.controller;

import ija.projekt.uml.model.*;
import ija.projekt.uml.view.movable.line.MovableLineWithMessage;

public class MessageController {
    private UMLMessage umlMessage;
    private MovableLineWithMessage line;

    public MessageController(UMLLifeline sender, UMLLifeline receiver, UMLOperation operation, MovableLineWithMessage line) {
        this(new UMLMessage(sender, receiver, operation), line);
    }

    public MessageController(UMLMessage umlMessage, MovableLineWithMessage line) {
        this.umlMessage = umlMessage;
        this.line = line;
    }

    public MovableLineWithMessage getLine() {
        return line;
    }

    public UMLMessage getUmlMessage() {
        return umlMessage;
    }

    public void updateMessage() {
        if(umlMessage.getSender() == null || umlMessage.getReceiver() == null || line == null) {
            return;
        }
        UMLOperation op = umlMessage.getOperation();
        if(op == null) {
            line.setMessage("<removed method>");
            return;
        }

        line.setMessage(op.getName());
    }
}
