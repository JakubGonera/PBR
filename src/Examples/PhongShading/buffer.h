#pragma once
#include "glad/glad.h"
#include "Application/utils.h"

class IndexBuffer {
private:
	unsigned int m_RendererID;
	unsigned int m_Count;
public:
	IndexBuffer(const unsigned int* data, unsigned int count){
        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
    }
	IndexBuffer() = default;
	~IndexBuffer(){
    }

	void FillBuffer(const unsigned int* data, unsigned int count){
        m_Count = count;
        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
        GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW));
    }
	void Bind() const{
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID));
    }
	void Unbind() const{
        GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
    }

	inline unsigned int GetCount() const { return m_Count; }
};

class VertexBuffer {
private:
	unsigned int m_RendererID;
public:
	VertexBuffer(const void* data, unsigned int size){
        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }
	VertexBuffer() = default;
	~VertexBuffer(){
    }

	void FillBuffer(const void* data, unsigned int size){
        GLCall(glGenBuffers(1, &m_RendererID));
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
        GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW));
    }
	void Bind() const{
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID));
    }
	void Unbind() const{
        GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));
    }
};