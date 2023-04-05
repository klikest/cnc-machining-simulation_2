#include"imgui.h"
#include"imgui_impl_glfw.h"
#include"imgui_impl_opengl3.h"

#include <iostream>
#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>

#include"shaderClass.h"
#include"VAO.h"
#include"VBO.h"
#include"EBO.h"
#include"Camera.h"
#include"Parser_obj.h"

#include <algorithm>
#include <chrono>
#include <iostream>

#define _USE_MATH_DEFINES
#include <math.h>


using namespace std::chrono;

const unsigned int width = 1600;
const unsigned int height = 900;


void draw_model(std::vector<float> vertices, std::vector<int> indices, VBO VBO_, VAO VAO_, EBO EBO_)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_.ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_.ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_DYNAMIC_DRAW);
	glBindVertexArray(VAO_.ID);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

}

void draw(Shader shader, VAO VAO_, std::vector<int> ind)
{
	shader.Activate();
	VAO_.Bind();
	glDrawElements(GL_TRIANGLES, ind.size(), GL_UNSIGNED_INT, 0);
}

void draw_model_coord(std::vector<float> vertices, std::vector<int> indices, VBO VBO_, VAO VAO_, EBO EBO_)
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO_.ID);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_.ID);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_DYNAMIC_DRAW);
	glBindVertexArray(VAO_.ID);
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

}

std::vector<float> rotatePointsAroundAxis(std::vector<float> points, std::vector<float> axis, double angle) {
	double cosAngle = cos(angle);
	double sinAngle = sin(angle);
	double a = axis[0];
	double b = axis[1];
	double c = axis[2];
	// Нормализуем ось вращения
	double length = sqrt(a * a + b * b + c * c);
	a /= length;
	b /= length;
	c /= length;
	// Вычисляем матрицу поворота
	double rotationMatrix[3][3] = {
		{ cosAngle + a * a * (1 - cosAngle),     a * b * (1 - cosAngle) - c * sinAngle, a * c * (1 - cosAngle) + b * sinAngle },
		{ b * a * (1 - cosAngle) + c * sinAngle,   cosAngle + b * b * (1 - cosAngle),   b * c * (1 - cosAngle) - a * sinAngle },
		{ c * a * (1 - cosAngle) - b * sinAngle,   c * b * (1 - cosAngle) + a * sinAngle, cosAngle + c * c * (1 - cosAngle)   }
	};
	std::vector<float> rot_points;
	for (int i = 0; i < points.size(); i += 6)
	{
		rot_points.push_back(rotationMatrix[0][0] * points[i] + rotationMatrix[0][1] * points[i+1] + rotationMatrix[0][2] * points[i+2]);
		rot_points.push_back(rotationMatrix[1][0] * points[i] + rotationMatrix[1][1] * points[i+1] + rotationMatrix[1][2] * points[i+2]);
		rot_points.push_back(rotationMatrix[2][0] * points[i] + rotationMatrix[2][1] * points[i+1] + rotationMatrix[2][2] * points[i+2]);
		rot_points.push_back(points[i+3]);
		rot_points.push_back(points[i+4]);
		rot_points.push_back(points[i+5]);
	}
	return rot_points;
}


std::vector<float> point = { 0,0,0 };
std::vector<float> start = { 0,0.5,0 };
std::vector<float> end = { 1,0.5,0 };


std::vector<int> get_ind(std::vector<float> vert)
{	
	std::vector<int> indices;
	for (int i = 0; i < vert.size()/6; ++i)
	{
		indices.push_back(i);
	}
	return indices;
}

std::vector<float> blank_vert;
std::vector<float> blank_vert_;
std::vector<int> blank_ind;

std::vector<float> p_b_vert;
std::vector<float> p_b_vert_;
std::vector<int> p_b_ind;

std::vector<float> table_vert;
std::vector<float> table_vert_;
std::vector<int> table_ind;

std::vector<float> tool_vert;
std::vector<float> tool_vert_;
std::vector<int> tool_ind;

std::vector<float> z_b_vert;
std::vector<float> z_b_vert_;
std::vector<int> z_b_ind;

std::vector<float> arrow_vert;
std::vector<float> arrow_vert_;
std::vector<int> arrow_ind;

std::vector<float> axis_vert;
std::vector<float> axis_vert_;
std::vector<int> axis_ind;


float x_Pos = 0;
float y_Pos = 0;
float z_Pos = 0;
float a_Pos = 0;
float c_Pos = 0;


void coords_to_zero(VBO VBO_blank, VBO VBO_p_b, VBO VBO_table, VBO VBO_tool, VBO VBO_z_b)
{
	/////////////////  X
	for (int i = 0; i < blank_vert.size(); i += 6) { blank_vert[i + 2] = blank_vert[i + 2] - x_Pos; }
	for (int i = 0; i < table_vert_.size(); i += 6) { table_vert[i + 2] = table_vert[i + 2] - x_Pos; }
	for (int i = 0; i < z_b_vert.size(); i += 6) { z_b_vert[i + 2] = z_b_vert[i + 2] - x_Pos; }

	///////////////////  Y
	for (int i = 0; i < p_b_vert_.size(); i += 6) { p_b_vert[i] = p_b_vert[i] + y_Pos; }
	for (int i = 0; i < tool_vert_.size(); i += 6) { tool_vert[i] = tool_vert[i] + y_Pos; }

	/////////////////  Z
	for (int i = 0; i < p_b_vert_.size(); i += 6) { p_b_vert[i + 1] = p_b_vert[i + 1] + z_Pos; }
	for (int i = 0; i < tool_vert_.size(); i += 6) { tool_vert[i + 1] = tool_vert[i + 1] + z_Pos; }

	x_Pos = 0;
	y_Pos = 0;
	z_Pos = 0;
}


void move(float x, float y, float z, VBO VBO_blank, VBO VBO_p_b, VBO VBO_table, VBO VBO_tool, VBO VBO_z_b )
{	///////////////////  X
	x_Pos += x;
	for (int i = 0; i < blank_vert.size(); i += 6)
	{
		blank_vert_[i+2] -= x;
	}
	for (int i = 0; i < table_vert_.size(); i += 6)
	{
		table_vert_[i + 2] -= x;
	}
	for (int i = 0; i < z_b_vert.size(); i += 6)
	{
		z_b_vert_[i + 2] -= x;
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_blank.ID);
	glBufferData(GL_ARRAY_BUFFER, blank_vert_.size() * sizeof(float), blank_vert_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_z_b.ID);
	glBufferData(GL_ARRAY_BUFFER, z_b_vert_.size() * sizeof(float), z_b_vert_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_table.ID);
	glBufferData(GL_ARRAY_BUFFER, table_vert_.size() * sizeof(float), table_vert_.data(), GL_DYNAMIC_DRAW);

	///////////////////  Y
	y_Pos = y;
	for (int i = 0; i < p_b_vert_.size(); i += 6)
	{
		p_b_vert_[i] += y;
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_p_b.ID);
	glBufferData(GL_ARRAY_BUFFER, p_b_vert_.size() * sizeof(float), p_b_vert_.data(), GL_DYNAMIC_DRAW);

	/////////////////  Z
	z_Pos += z;
	for (int i = 0; i < p_b_vert_.size(); i += 6)
	{
		p_b_vert_[i+1] += z;
	}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_p_b.ID);
	glBufferData(GL_ARRAY_BUFFER, p_b_vert_.size() * sizeof(float), p_b_vert_.data(), GL_DYNAMIC_DRAW);
}

void move_G(float x, float y, float z, float c, VBO VBO_blank, VBO VBO_p_b, VBO VBO_table, VBO VBO_tool, VBO VBO_z_b)
{	///////////////////  X
	x_Pos = x;
	//for (int i = 0; i < blank_vert.size(); i += 6) { blank_vert_[i + 2] = blank_vert[i+2] - x;}
	for (int i = 0; i < table_vert_.size(); i += 6){table_vert_[i + 2] = table_vert[i+2] - x;}
	for (int i = 0; i < z_b_vert.size(); i += 6){z_b_vert_[i + 2] = z_b_vert[i+2] - x;}
	//glBindBuffer(GL_ARRAY_BUFFER, VBO_blank.ID);
	//glBufferData(GL_ARRAY_BUFFER, blank_vert_.size() * sizeof(float), blank_vert_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_z_b.ID);
	glBufferData(GL_ARRAY_BUFFER, z_b_vert_.size() * sizeof(float), z_b_vert_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_table.ID);
	glBufferData(GL_ARRAY_BUFFER, table_vert_.size() * sizeof(float), table_vert_.data(), GL_DYNAMIC_DRAW);

	///////////////////  Y
	y_Pos = y;
	for (int i = 0; i < p_b_vert_.size(); i += 6){p_b_vert_[i] = p_b_vert[i] + y;}
	for (int i = 0; i < tool_vert_.size(); i += 6){tool_vert_[i] = tool_vert[i] + y;}

	/////////////////  Z
	z_Pos = z;
	for (int i = 0; i < p_b_vert_.size(); i += 6){p_b_vert_[i + 1] = p_b_vert[i+1] + z;}
	for (int i = 0; i < tool_vert_.size(); i += 6){tool_vert_[i+1] = tool_vert[i+1] + z;}
	glBindBuffer(GL_ARRAY_BUFFER, VBO_p_b.ID);
	glBufferData(GL_ARRAY_BUFFER, p_b_vert_.size() * sizeof(float), p_b_vert_.data(), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_tool.ID);
	glBufferData(GL_ARRAY_BUFFER, tool_vert_.size() * sizeof(float), tool_vert_.data(), GL_DYNAMIC_DRAW);

	/////////////////  c
	c_Pos = c;
	blank_vert_ = rotatePointsAroundAxis(blank_vert, { 0,1,0 }, -M_PI/2 );
	blank_vert_ = rotatePointsAroundAxis(blank_vert_, { 1,0,0 }, -c_Pos * M_PI / 180);

	blank_vert_ = rotatePointsAroundAxis(blank_vert_, { 0,1,0 }, M_PI / 2);
	for (int i = 0; i < blank_vert_.size(); i += 6)
	{
		blank_vert_[i + 1] += 100;
	}
	for (int i = 0; i < blank_vert_.size(); i += 6) { blank_vert_[i + 2] = blank_vert_[i + 2] - x; }
	glBindBuffer(GL_ARRAY_BUFFER, VBO_blank.ID);
	glBufferData(GL_ARRAY_BUFFER, blank_vert_.size() * sizeof(float), blank_vert_.data(), GL_DYNAMIC_DRAW);

}



void start_g_code(int i, std::vector<std::vector<float>> g_code)
{
	x_Pos = g_code[i][0];
	y_Pos = g_code[i][1];
	z_Pos = g_code[i][2];
	c_Pos = g_code[i][3];
}



std::vector<std::vector<float>> g_code;


int main()
{

	for (float i = 0; i < 45; i += 0.2)
	{
		g_code.push_back({55 +i, 49, -9.8, 0 });
	}


	for (float i = 0; i < 45; i += 0.2)
	{
		g_code.push_back({ 55 +45 - i, 49, -9.8, 0 });
	}


	for (float i = 0; i < 43; i += 0.2)
	{
		g_code.push_back({ 55 , 49, -9.8, i });
	}

	for (float i = 0; i < 45; i += 0.2)
	{
		g_code.push_back({ 55 + i, 49, -9.8, 45 });
	}


	for (float i = 0; i < 45; i += 0.2)
	{
		g_code.push_back({ 55 + 45 - i, 49, -9.8, 45 });
	}






	blank_vert = Parse_vertices_blank();
	
	for (int i = 0; i < blank_vert.size(); i += 6)
	{
		blank_vert[i] += 25;
	}

	blank_vert = rotatePointsAroundAxis(blank_vert, { 0,1,0 }, M_PI / 2);
	blank_vert_ = blank_vert;



	for (int i = 0; i < blank_vert_.size(); i += 6)
	{
		blank_vert_[i+1] += 100;
	}
	
	blank_ind = get_ind(blank_vert);

	p_b_vert = Parse_vertices_p_babka();
	p_b_vert = rotatePointsAroundAxis(p_b_vert, { 1,0,0 }, -M_PI / 2);
	p_b_vert_ = p_b_vert;
	p_b_ind = get_ind(p_b_vert);

	table_vert = Parse_vertices_table();
	table_vert = rotatePointsAroundAxis(table_vert, { 1,0,0 }, -M_PI / 2);
	table_vert_ = table_vert;
	table_ind = get_ind(table_vert);

	tool_vert = Parse_vertices_tool();
	tool_vert = rotatePointsAroundAxis(tool_vert, { 1,0,0 }, -M_PI / 2);
	tool_vert_ = tool_vert;
	tool_ind = get_ind(tool_vert);

	z_b_vert = Parse_vertices_z_babka();
	z_b_vert = rotatePointsAroundAxis(z_b_vert, { 1,0,0 }, -M_PI / 2);
	z_b_vert_ = z_b_vert;
	z_b_ind = get_ind(z_b_vert);

	arrow_vert = Parse_vertices_axis();
	arrow_vert = rotatePointsAroundAxis(arrow_vert, { 1,0,0 }, -M_PI / 2);
	arrow_vert_ = arrow_vert;
	arrow_ind = get_ind(arrow_vert);

	// Initialize GLFW
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* window = glfwCreateWindow(width, height, "cnc machining sinulator", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	gladLoadGL();
	glViewport(0, 0, width, height);

	Shader blank_shaderProgram("blank.vert", "blank.frag");
	VAO VAO_blank;
	VAO_blank.Bind();
	VBO VBO_blank(blank_vert_.data(), blank_vert_.size() * sizeof(float));
	EBO EBO_blank(blank_ind.data(), blank_ind.size() * sizeof(int));
	VAO_blank.LinkAttrib(VBO_blank, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO_blank.LinkAttrib(VBO_blank, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_blank.Unbind();
	VBO_blank.Unbind();
	EBO_blank.Unbind();
	
	Shader tool_shaderProgram("tool.vert", "tool.frag");
	VAO VAO_tool;
	VAO_tool.Bind();
	VBO VBO_tool(tool_vert_.data(), tool_vert_.size() * sizeof(float));
	EBO EBO_tool(tool_ind.data(), tool_ind.size() * sizeof(int));
	VAO_tool.LinkAttrib(VBO_tool, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO_tool.LinkAttrib(VBO_tool, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_tool.Unbind();
	VBO_tool.Unbind();
	EBO_tool.Unbind();

	Shader p_b_shaderProgram("p_b.vert", "p_b.frag");
	VAO VAO_p_b;
	VAO_p_b.Bind();
	VBO VBO_p_b(p_b_vert_.data(), p_b_vert_.size() * sizeof(float));
	EBO EBO_p_b(p_b_ind.data(), p_b_ind.size() * sizeof(int));
	VAO_p_b.LinkAttrib(VBO_p_b, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO_p_b.LinkAttrib(VBO_p_b, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_p_b.Unbind();
	VBO_p_b.Unbind();
	EBO_p_b.Unbind();

	Shader table_shaderProgram("table.vert", "table.frag");
	VAO VAO_table;
	VAO_table.Bind();
	VBO VBO_table(table_vert_.data(), table_vert_.size() * sizeof(float));
	EBO EBO_table(table_ind.data(), table_ind.size() * sizeof(int));
	VAO_table.LinkAttrib(VBO_table, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO_table.LinkAttrib(VBO_table, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_table.Unbind();
	VBO_table.Unbind();
	EBO_table.Unbind();



	Shader z_b_shaderProgram("z_b.vert", "z_b.frag");
	VAO VAO_z_b;
	VAO_z_b.Bind();
	VBO VBO_z_b(z_b_vert_.data(), z_b_vert_.size() * sizeof(float));
	EBO EBO_z_b(z_b_ind.data(), z_b_ind.size() * sizeof(int));
	VAO_z_b.LinkAttrib(VBO_z_b, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO_z_b.LinkAttrib(VBO_z_b, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_z_b.Unbind();
	VBO_z_b.Unbind();
	EBO_z_b.Unbind();




	for (int i = 0; i < arrow_vert.size(); i += 6)
	{
		arrow_vert[i + 3] = 1;
		arrow_vert[i + 4] = 0;
		arrow_vert[i + 5] = 0;
	}

	for (int i = 0; i < arrow_vert.size(); ++i)
	{
		axis_vert.push_back(arrow_vert[i]);
	}
	//////////////////////

	arrow_vert = rotatePointsAroundAxis(arrow_vert, { 0, 1, 0 }, M_PI / 2);
	for (int i = 0; i < arrow_vert.size(); i += 6)
	{
		arrow_vert[i + 3] = 0;
		arrow_vert[i + 4] = 1;
		arrow_vert[i + 5] = 0;
	}

	for (int i = 0; i < arrow_vert.size(); ++i)
	{
		axis_vert.push_back(arrow_vert[i]);
	}
	/// ////////////////////
	arrow_vert = rotatePointsAroundAxis(arrow_vert, { 1, 0, 0 }, M_PI / 2);
	for (int i = 0; i < arrow_vert.size(); i += 6)
	{
		arrow_vert[i + 3] = 0;
		arrow_vert[i + 4] = 0;
		arrow_vert[i + 5] = 1;
	}

	for (int i = 0; i < arrow_vert.size(); ++i)
	{
		axis_vert.push_back(arrow_vert[i]);
	}


	axis_ind = get_ind(axis_vert);
	
	Shader cords_shaderProgram("cords.vert", "cords.frag");
	VAO VAO_cords;
	VAO_cords.Bind();
	VBO VBO_cords(axis_vert.data(), axis_vert.size() * sizeof(float));
	EBO EBO_cords(axis_ind.data(), axis_ind.size() * sizeof(int));
	VAO_cords.LinkAttrib(VBO_cords, 0, 3, GL_FLOAT, 6 * sizeof(float), (void*)0);
	VAO_cords.LinkAttrib(VBO_cords, 1, 3, GL_FLOAT, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	VAO_cords.Unbind();
	VBO_cords.Unbind();
	EBO_cords.Unbind();
	
	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(500.0f, 500.0f, 500.0f);

	glm::vec3 toolPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 toolModel = glm::mat4(1.0f);
	toolModel = glm::translate(toolModel, toolPos);

	glm::vec3 blankPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 blankModel = glm::mat4(1.0f);
	blankModel = glm::translate(blankModel, blankPos);

	glm::vec3 cordsPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 cordsModel = glm::mat4(1.0f);
	cordsModel = glm::translate(cordsModel, cordsPos);



	blank_shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(blank_shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(blankModel));
	glUniform4f(glGetUniformLocation(blank_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(blank_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	tool_shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(tool_shaderProgram.ID, "model_tool"), 1, GL_FALSE, glm::value_ptr(toolModel));
	glUniform4f(glGetUniformLocation(tool_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(tool_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	table_shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(table_shaderProgram.ID, "model_tool"), 1, GL_FALSE, glm::value_ptr(toolModel));
	glUniform4f(glGetUniformLocation(table_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(table_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	z_b_shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(z_b_shaderProgram.ID, "model_tool"), 1, GL_FALSE, glm::value_ptr(toolModel));
	glUniform4f(glGetUniformLocation(z_b_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(z_b_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

	p_b_shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(p_b_shaderProgram.ID, "model_tool"), 1, GL_FALSE, glm::value_ptr(toolModel));
	glUniform4f(glGetUniformLocation(p_b_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(p_b_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);


	cords_shaderProgram.Activate();
	glUniformMatrix4fv(glGetUniformLocation(cords_shaderProgram.ID, "model_cords"), 1, GL_FALSE, glm::value_ptr(cordsModel));


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	

	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 2.0f));

	int frame = 0;

	double prevTime = 0.0f;
	double crntTime = 0.0f;
	double timeDiff = 0.0f;
	unsigned int counter = 0;


	bool draw_cords = false;
	bool draw_src_mesh = false;
	bool draw_scene = false;
	bool draw_g = false;

	glfwSwapInterval(1);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Arial.ttf", 20);
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 330");



	int max_it = g_code.size();
	int now_it = 0;


	while (!glfwWindowShouldClose(window))
	{

		crntTime = glfwGetTime();
		timeDiff = crntTime - prevTime;
		counter++;

		if (timeDiff >= 1.0 / 30.0)
		{
			std::string FPS = std::to_string((1.0 / timeDiff) * counter);
			std::string title = "cnc mashining simulator - " + FPS + "FPS";
			glfwSetWindowTitle(window, title.c_str());
			prevTime = crntTime;
			counter = 0;
		}

		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		
		camera.Inputs(window);
		camera.updateMatrix(45.0f, 0.01f, 10000.0f);


		tool_shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(tool_shaderProgram.ID, "model"), 1, GL_FALSE, glm::value_ptr(toolModel));
		glUniform4f(glGetUniformLocation(tool_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(tool_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(tool_shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(tool_shaderProgram, "camMatrix");

		blank_shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(blank_shaderProgram.ID, "model_b"), 1, GL_FALSE, glm::value_ptr(blankModel));
		glUniform4f(glGetUniformLocation(blank_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(blank_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(blank_shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(blank_shaderProgram, "camMatrix");


		p_b_shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(p_b_shaderProgram.ID, "model_b"), 1, GL_FALSE, glm::value_ptr(blankModel));
		glUniform4f(glGetUniformLocation(p_b_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(p_b_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(p_b_shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(p_b_shaderProgram, "camMatrix");

		z_b_shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(z_b_shaderProgram.ID, "model_b"), 1, GL_FALSE, glm::value_ptr(blankModel));
		glUniform4f(glGetUniformLocation(z_b_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(z_b_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(z_b_shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(z_b_shaderProgram, "camMatrix");

		table_shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(table_shaderProgram.ID, "model_b"), 1, GL_FALSE, glm::value_ptr(blankModel));
		glUniform4f(glGetUniformLocation(table_shaderProgram.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
		glUniform3f(glGetUniformLocation(table_shaderProgram.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
		glUniform3f(glGetUniformLocation(table_shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(table_shaderProgram, "camMatrix");

		cords_shaderProgram.Activate();
		glUniformMatrix4fv(glGetUniformLocation(cords_shaderProgram.ID, "model_b"), 1, GL_FALSE, glm::value_ptr(blankModel));
		glUniform3f(glGetUniformLocation(cords_shaderProgram.ID, "camPos"), camera.Position.x, camera.Position.y, camera.Position.z);
		camera.Matrix(cords_shaderProgram, "camMatrix");


		move_G(x_Pos, y_Pos, z_Pos, c_Pos, VBO_blank, VBO_p_b, VBO_table, VBO_tool, VBO_z_b);

		if (draw_scene)
		{
			draw(blank_shaderProgram, VAO_blank, blank_ind);
			draw(tool_shaderProgram, VAO_tool, tool_ind);
			draw(p_b_shaderProgram, VAO_p_b, p_b_ind);
			draw(z_b_shaderProgram, VAO_z_b, z_b_ind);
			draw(table_shaderProgram, VAO_table, table_ind);
		}
		
		if (draw_cords)
		{
			draw(cords_shaderProgram, VAO_cords, axis_ind);
		}

		
		std::string x_cord = "X = " + std::to_string(x_Pos);
		std::string y_cord = "Y = " + std::to_string(y_Pos);
		std::string z_cord = "Z = " + std::to_string(z_Pos);
		std::string a_cord = "A = " + std::to_string(a_Pos);
		std::string c_cord = "C = " + std::to_string(c_Pos);



		ImGui::Begin("Processing info");
		ImGui::Checkbox("Draw scene", &draw_scene);
		ImGui::Checkbox("Draw coords", &draw_cords);
		ImGui::SliderFloat("X_coord", &x_Pos, -150.0f, 150.0f);
		ImGui::SliderFloat("Y_coord", &y_Pos, -150.0f, 150.0f);
		ImGui::SliderFloat("Z_coord", &z_Pos, -150.0f, 150.0f);
		ImGui::SliderFloat("A_coord", &a_Pos, -360.0f, 360.0f);
		ImGui::SliderFloat("C_coord", &c_Pos, -360.0f, 360.0f);

		ImGui::Text(x_cord.c_str());
		ImGui::Text(y_cord.c_str());
		ImGui::Text(z_cord.c_str());
		ImGui::Text(a_cord.c_str());
		ImGui::Text(c_cord.c_str());

		if (ImGui::Button("Set to zero"))
			coords_to_zero(VBO_blank, VBO_p_b, VBO_table, VBO_tool, VBO_z_b);

		ImGui::Checkbox("Draw G", &draw_g);
		ImGui::End();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


		if (draw_g)
		{
			start_g_code(now_it, g_code);
			if (now_it < max_it-1)
			{
				now_it += 1;
			}
			else
			{
				now_it = 0;
			}
		}
			
		
				
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	VAO_blank.Delete();
	VBO_blank.Delete();
	EBO_blank.Delete();
	blank_shaderProgram.Delete();
	VAO_tool.Delete();
	VBO_tool.Delete();
	EBO_tool.Delete();
	tool_shaderProgram.Delete();
	VAO_cords.Delete();
	VBO_cords.Delete();
	EBO_cords.Delete();
	cords_shaderProgram.Delete();
	VAO_table.Delete();
	VBO_table.Delete();
	EBO_table.Delete();
	table_shaderProgram.Delete();
	VAO_p_b.Delete();
	VBO_p_b.Delete();
	EBO_p_b.Delete();
	p_b_shaderProgram.Delete();
	VAO_z_b.Delete();
	VBO_z_b.Delete();
	EBO_z_b.Delete();
	z_b_shaderProgram.Delete();

	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}