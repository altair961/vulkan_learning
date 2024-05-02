#pragma once

#include "lve_device.h"
#include "lve_pipeline.h"
#include "lve_swap_chain.h"
#include "lve_window.h" 
#include "lve_model.h" 
#include <memory>
#include "triangle.h"
#include "triangle_location.h"

namespace lve {
	class FirstApp {

	public:
		static constexpr int WIDTH = 800;
		static constexpr int HEIGHT = 600;

		FirstApp();
		~FirstApp();

		FirstApp(const FirstApp&) = delete;
		FirstApp& operator=(const FirstApp&) = delete;

		void run();

	private:
		void loadModels();
		void createPipelineLayout();
		void createPipeline();
		void createCommandBuffers();
		void drawFrame();
		Triangle getTriangle(lve::TriangleLocation triangleLocation, 
			glm::vec2 topVertex, glm::vec2 rightVertex, glm::vec2 leftVertex);
		void getOneStepVertices(
			std::vector<LveModel::Vertex> &vertices, 
			int depth, 
			glm::vec2 topVert,
			glm::vec2 rightVert,
			glm::vec2 leftVert
		);
		glm::vec2 getMiddleVertex(glm::vec2 firstVertex, glm::vec2 secondVertex);

		LveWindow lveWindow{ WIDTH, HEIGHT, "Hello Vulkan!" };
		LveDevice lveDevice{lveWindow};
		LveSwapChain lveSwapChain{lveDevice, lveWindow.getExtent()};
		std::unique_ptr<LvePipeline> lvePipeline;
		VkPipelineLayout pipelineLayout;
		std::vector<VkCommandBuffer> commandBuffers;
		std::unique_ptr<LveModel> lveModel;
	};
}