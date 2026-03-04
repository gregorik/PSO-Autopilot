<h1>PSO Autopilot (Core)</h1>
<h2>Solution to Unreal Engine 5 OOM Crashes</h2>
<p>The few &quot;Shader Compilation&quot; plugins that exist on marketplaces brute-force load all of your assets at once. If you have a massive 50GB project, this will cause your RAM to spike, your loading screen to freeze entirely, and eventually result in an Out-Of-Memory (OOM) crash.</p>
<p><strong>PSO Autopilot fixes this.</strong></p>
<h3>The Core Feature: Memory-Safe Chunking</h3>
<p>Instead of loading thousands of materials simultaneously, PSO Autopilot Core uses the FStreamableManager to <strong>asynchronously stream</strong> your Materials in controlled batches (e.g., 100 at a time). After a batch is compiled via the Engine&#39;s native FShaderPipelineCache, the plugin explicitly drops references and yields to Unreal&#39;s asynchronous Garbage Collection. Your RAM footprint remains completely flat, making this tool safe for massive open-world projects.</p>
<hr>
<hr>
<h2>How to Use</h2>
<ol>
<li>Copy the PSOAutopilotCore folder into your project&#39;s Plugins folder.</li>
<li>Regenerate Visual Studio files and compile.</li>
<li>Open Unreal Engine. Go to <strong>Edit &gt; Project Settings &gt; PSO Autopilot Core</strong>. Set your target directories and batch sizes.</li>
<li>In your Loading Screen UI Blueprint, call Get Game Instance Subsystem -&gt; PSO Autopilot Core Subsystem.  </li>
<li>Bind an event to On Progress Updated to drive your loading bar.</li>
<li>Call Start Warmup.</li>
</ol>
<p><em>Code by Andras Gregori. MIT License.</em></p>
