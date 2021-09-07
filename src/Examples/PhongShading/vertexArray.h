#pragma once
#include "buffer.h"
#include "vertexBufferLayout.h"
#include "Application/utils.h"

class VertexBufferLayout;

class VertexArray {
private:
	unsigned int m_RendererID;
	unsigned int m_attribCounter = 0;
public:
	VertexArray(){
        GLCall(glGenVertexArrays(1, &m_RendererID));
    }
	~VertexArray(){
        GLCall(glDeleteVertexArrays(1, &m_RendererID));
    }

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout){
        Bind();
        vb.Bind();

        const auto& elements = layout.GetElements();
        unsigned int offset = 0;
        for (unsigned int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];
            GLCall(glEnableVertexAttribArray(m_attribCounter));
            GLCall(glVertexAttribPointer(m_attribCounter, element.count, element.type, element.normalized, layout.GetStride(), reinterpret_cast<GLvoid *>(offset)));
            offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
            m_attribCounter++;
        }
    }

	void Bind() const{
        GLCall(glBindVertexArray(m_RendererID));
    }
	void Unbind() const{
        GLCall(glBindVertexArray(0));
    }
};