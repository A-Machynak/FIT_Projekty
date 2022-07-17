package ija.projekt.uml.utils;

import java.util.ArrayList;

public class FunctionDeclaration {
    private final String name;
    private final String accessModifier;
    private ArrayList<Pair<String, String>> arguments = new ArrayList<>();

    public FunctionDeclaration(String name, String accessModifier) {
        this.name = name;
        this.accessModifier = accessModifier;
    }

    public void addArgument(String name, String type) {
        arguments.add(new Pair<>(name, type));
    }

    public String getName() {
        return name;
    }

    public String getAccessModifier() {
        return accessModifier;
    }

    public ArrayList<Pair<String, String>> getArguments() {
        return arguments;
    }

    @Override
    public String toString() {
        return "FunctionDeclaration{" +
                "name='" + name + '\'' +
                ", accessModifier='" + accessModifier + '\'' +
                ", arguments=" + arguments +
                '}';
    }
}
