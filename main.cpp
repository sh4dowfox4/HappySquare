#include "raylib.h"
#include "raymath.h"

const int screenWidth = 1920;
const int screenHeight = 1080;

const int worldWidth = 50000;
const int worldHeight = 50000;

const int fps = 180;

int main(void) {
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	InitWindow(screenWidth, screenHeight, "42");

	SetWindowPosition(1920, 0); // КОСТЫЛЬ ДЛЯ МОНИТОРА
	//SetWindowState(FLAG_FULLSCREEN_MODE);

	SetTargetFPS(fps);

	Vector2 ballPos = { worldWidth / 2, worldHeight / 2 };
	const float ballSpeed = 1000.f;
	const int ballRadius = 15;

	Camera2D camera = { 0 };
	camera.target = ballPos;
	camera.offset = (Vector2){ screenWidth / 2, screenHeight / 2 };
	camera.rotation = { 0.f };
	
	float zoomStart = (float)screenWidth / worldWidth;
	if ((float)screenHeight / worldHeight < zoomStart) {
		zoomStart = (float)screenHeight / worldHeight;
	}
	camera.zoom = zoomStart;
	float targetZoom = 1.f;
	float zoomSpeed = 1.f;
	
	while(!WindowShouldClose()) {
		float frameTime = GetFrameTime();
		float delta = frameTime * ballSpeed;

		if (IsKeyDown(KEY_W)) ballPos.y -= delta;
		if (IsKeyDown(KEY_A)) ballPos.x -= delta;
		if (IsKeyDown(KEY_S)) ballPos.y += delta;
		if (IsKeyDown(KEY_D)) ballPos.x += delta;

		if (ballPos.x < ballRadius) ballPos.x = ballRadius;
		if (ballPos.x > worldWidth - ballRadius) ballPos.x = worldWidth - ballRadius;
		if (ballPos.y < ballRadius) ballPos.y = ballRadius;
		if (ballPos.y > worldHeight - ballRadius) ballPos.y = worldHeight - ballRadius;

		camera.target = ballPos;
		float wheel = GetMouseWheelMove();

    	if (wheel != 0) {
        	targetZoom += wheel * 0.1f;
	  		if (targetZoom < 0.1f) targetZoom = 0.1f;
        	if (targetZoom > 5.0f) targetZoom = 5.0f;
    	}
		camera.zoom = Lerp(camera.zoom, targetZoom, zoomSpeed * frameTime);

		BeginDrawing();
			ClearBackground(BLACK);
	//		int Fps = GetFPS();
	//		DrawText(TextFormat("FrameTime:%f\tFPS: %i", frameTime, Fps), 20, 10, 25, BLUE);
			DrawText(TextFormat("x: %i, y: %i", (int)ballPos.x, (int)ballPos.y), 20, 50, 30, GREEN);
			DrawText(TextFormat("camera.zoom: %f\ntargetZoom: %f", camera.zoom, targetZoom), 20, 90, 30, GREEN);
			DrawText(TextFormat("World size: %ix%i\nResolution: %ix%i",
					worldWidth, worldHeight, screenWidth,screenHeight),
				   	20, screenHeight-100, 25, YELLOW);
			
			BeginMode2D(camera);
				DrawCircleV(ballPos, float(ballRadius), RED);

		        for (int i = 0; i <= worldWidth; i += 100) {
        		    DrawLine(i, 0, i, worldHeight, GRAY);
        		}
        		for (int i = 0; i <= worldHeight; i += 100) {
            		DrawLine(0, i, worldWidth, i, GRAY);
        		}
				DrawRectangleLines (0, 0, worldWidth, worldHeight, WHITE);
			EndMode2D();
		EndDrawing();
	}
	CloseWindow();
}
