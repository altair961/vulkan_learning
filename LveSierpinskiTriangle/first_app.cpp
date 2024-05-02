#include "first_app.h"
#include <stdexcept>
#include <array>
#include <iostream>

namespace lve {
	FirstApp::FirstApp()
	{
		loadModels();
		createPipelineLayout();
		createPipeline();
		createCommandBuffers();
	}

	FirstApp::~FirstApp()
	{
		vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
	}

	void FirstApp::run()
	{
		while (!lveWindow.shouldClose())
		{
			glfwPollEvents();
			drawFrame();
		}

		vkDeviceWaitIdle(lveDevice.device());
	}

	void FirstApp::loadModels()
	{
		std::vector<LveModel::Vertex> vertices;
		getOneStepVertices(vertices, 6, { 0.0f, -0.5f }, { 0.5f, 0.5f }, { -0.5f, 0.5f });
		lveModel = std::make_unique<LveModel>(lveDevice, vertices);
	}


	void FirstApp::createPipelineLayout()
	{
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0;
		pipelineLayoutInfo.pSetLayouts = nullptr;
		pipelineLayoutInfo.pushConstantRangeCount = 0;
		pipelineLayoutInfo.pPushConstantRanges = nullptr;
		if (vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo,
			nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}
	}

	void FirstApp::createPipeline()
	{
		auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(
			lveSwapChain.width(), lveSwapChain.height());
		pipelineConfig.renderPass = lveSwapChain.getRenderPass();
		pipelineConfig.pipelineLayout = pipelineLayout;
		lvePipeline = std::make_unique<LvePipeline>(
			lveDevice,
			"shaders/simple_shader.vert.spv",
			"shaders/simple_shader.frag.spv",
			pipelineConfig);
	}

	void FirstApp::createCommandBuffers()
	{
		commandBuffers.resize(lveSwapChain.imageCount());

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = lveDevice.getCommandPool();
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo,
			commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}

		for (int i = 0; i < commandBuffers.size(); i++)
		{
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

			if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			// first command we're going to record is "begin a render pass"
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = lveSwapChain.getRenderPass();
			renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

			renderPassInfo.renderArea.offset = { 0, 0 };
			renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

			std::array<VkClearValue, 2> clearValues{};
			clearValues[0].color = { 0.1f, 0.1f, 0.1f, 1.0f };
			clearValues[1].depthStencil = { 0.1f, 0 };
			renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
			renderPassInfo.pClearValues = clearValues.data();

			// record command into current command buffer to begin this render pass
			vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

			lvePipeline->bind(commandBuffers[i]);
			lveModel->bind(commandBuffers[i]);
			lveModel->draw(commandBuffers[i]);

			vkCmdEndRenderPass(commandBuffers[i]);
			if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}
	}

	void FirstApp::drawFrame()
	{
		uint32_t imageIndex;
		auto result = lveSwapChain.acquireNextImage(&imageIndex);

		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}
		result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}
	}

	void FirstApp::getOneStepVertices(
		std::vector<LveModel::Vertex>& vertices,
		int depth,
		glm::vec2 topVert,
		glm::vec2 rightVert,
		glm::vec2 leftVert
	)
	{
		std::cout << "depth: " << depth << std::endl;

		depth--;
		if (depth > 0) 
		{
			Triangle topTriangle = getTriangle(
				lve::TriangleLocation::TopTriangle, topVert, rightVert, leftVert);
			Triangle rightTriangle = getTriangle(
				lve::TriangleLocation::RightTriangle, topVert, rightVert, leftVert);
			Triangle leftTriangle = getTriangle(
				lve::TriangleLocation::LeftTriangle, topVert, rightVert, leftVert);

			getOneStepVertices(vertices, depth, topTriangle.mTopVertex, topTriangle.mRightVertex, topTriangle.mLeftVertex);
			getOneStepVertices(vertices, depth, rightTriangle.mTopVertex, rightTriangle.mRightVertex, rightTriangle.mLeftVertex);
			getOneStepVertices(vertices, depth, leftTriangle.mTopVertex, leftTriangle.mRightVertex, leftTriangle.mLeftVertex);
		}
		else if (depth == 0)
		{
			Triangle topTriangle = getTriangle(
				lve::TriangleLocation::TopTriangle, topVert, rightVert, leftVert);
			Triangle rightTriangle = getTriangle(
				lve::TriangleLocation::RightTriangle, topVert, rightVert, leftVert);
			Triangle leftTriangle = getTriangle(
				lve::TriangleLocation::LeftTriangle, topVert, rightVert, leftVert);

			LveModel::Vertex topTriangleTopVertex;
			topTriangleTopVertex.position = topTriangle.mTopVertex;
			LveModel::Vertex topTriangleRightVertex;
			topTriangleRightVertex.position = topTriangle.mRightVertex;
			LveModel::Vertex topTriangleLeftVertex;
			topTriangleLeftVertex.position = topTriangle.mLeftVertex;
			
			LveModel::Vertex rightTriangleTopVertex;
			rightTriangleTopVertex.position = rightTriangle.mTopVertex;
			LveModel::Vertex rightTriangleRightVertex;
			rightTriangleRightVertex.position = rightTriangle.mRightVertex;
			LveModel::Vertex rightTriangleLeftVertex;
			rightTriangleLeftVertex.position = rightTriangle.mLeftVertex;
			
			LveModel::Vertex leftTriangleTopVertex;
			leftTriangleTopVertex.position = leftTriangle.mTopVertex;
			LveModel::Vertex leftTriangleRightVertex;
			leftTriangleRightVertex.position = leftTriangle.mRightVertex;
			LveModel::Vertex leftTriangleLeftVertex;
			leftTriangleLeftVertex.position = leftTriangle.mLeftVertex;

			vertices.push_back(topTriangleTopVertex);
			vertices.push_back(topTriangleRightVertex);
			vertices.push_back(topTriangleLeftVertex);

			vertices.push_back(rightTriangleTopVertex);
			vertices.push_back(rightTriangleRightVertex);
			vertices.push_back(rightTriangleLeftVertex);

			vertices.push_back(leftTriangleTopVertex);
			vertices.push_back(leftTriangleRightVertex);
			vertices.push_back(leftTriangleLeftVertex);
		}
	}

	glm::vec2 FirstApp::getMiddleVertex(glm::vec2 firstVertex, glm::vec2 secondVertex)
	{
		auto resultX = (firstVertex.x + secondVertex.x) / 2;
		auto resultY = (firstVertex.y + secondVertex.y) / 2;
		glm::vec2 midVert = { resultX , resultY };

		return midVert;
	}

	Triangle FirstApp::getTriangle(lve::TriangleLocation triangleLocation,
		glm::vec2 topVertex, glm::vec2 rightVertex, glm::vec2 leftVertex)
	{
		if (triangleLocation == lve::TriangleLocation::TopTriangle) 
		{
			auto topTriangle = lve::Triangle(topVertex, getMiddleVertex(topVertex,
				rightVertex), getMiddleVertex(leftVertex, topVertex));
			return topTriangle;
		}

		if (triangleLocation == lve::TriangleLocation::RightTriangle)
		{
			auto rightTriangle = lve::Triangle(getMiddleVertex(topVertex, rightVertex),
				rightVertex, getMiddleVertex(rightVertex, leftVertex));
			return rightTriangle;
		}

		if (triangleLocation == lve::TriangleLocation::LeftTriangle)
		{
			auto leftTriangle = lve::Triangle(getMiddleVertex(leftVertex, topVertex),
				getMiddleVertex(rightVertex, leftVertex), leftVertex);
			return leftTriangle;
		}
	}
}