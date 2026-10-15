
extern "C" void app_main()
{
    SystemOrchestrator orchestrator;
    App app(orchestrator);
    app.run();
}