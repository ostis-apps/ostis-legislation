package net.ostis.jesc.client.model.response;

import lombok.Data;

import java.util.List;

@Data
public class ScResponse<T> {

    private Long id;

    private Boolean status;

    private Boolean event;

    private T payload;

    private List<String> errors;

}
