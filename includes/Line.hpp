#pragma once

#include "Graphics.hpp"
#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class Line {
	unsigned int VBO, VAO;
	std::vector<float> vertices;
	glm::vec3 startPoint;
	glm::vec3 endPoint;
	glm::mat4 MVP;
	glm::vec3 lineColor;
	bool		isActive;
public:
	Line(glm::vec3 start, glm::vec3 end, bool isActive = true):isActive(isActive) {

		startPoint = start;
		endPoint = end;
		lineColor = glm::vec3(1,1,1);
		MVP = glm::mat4(1.0f);

		vertices = {
			 start.x, start.y, start.z,
			 end.x, end.y, end.z,
		};
		
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		// glLineWidth(3);
		glBindBuffer(GL_ARRAY_BUFFER, 0); 
		glBindVertexArray(0); 

	}

	int setMVP(glm::mat4 mvp) {
		MVP = mvp;
		return 1;
	}

	int setColor(glm::vec3 color) {
		lineColor = color;
		return 1;
	}

	int  setPosition(glm::vec3 new_start, glm::vec3 new_end)
	{
		vertices = {
			 new_start.x, new_start.y, new_start.z,
			 new_end.x, new_end.y, new_end.z,
		};

		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
		//BIND VAO and VBO 0
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	int render(Shader* shader)
	{
		if (!isActive)
			return 1;
		shader->use();
		glUniformMatrix4fv(glGetUniformLocation(shader->ID, "MVP"), 1, GL_FALSE, &MVP[0][0]);
		glUniform3fv(glGetUniformLocation(shader->ID, "color"), 1, &lineColor[0]);

		glBindVertexArray(VAO);
		glDrawArrays(GL_LINES, 0, 2);
		glBindVertexArray(0);
    	glUseProgram(0);
		return 1;
	}

	~Line() {

		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(1, &VBO);
	}
	void activate() { this->isActive = true; }
	void deactivate() { this->isActive = false; }

};
