// Answers the plug-in's calls in a plain browser, so the UI can be developed, tested and photographed without a DAW.

export async function mockCall(method: string, params: Record<string, unknown>): Promise<unknown> {
  switch (method) {
    case 'ui.hello':
      return { kind: 'instrument', version: '0.1.0-alpha (mock)', host: 'Browser', platform: 'web' }
    case 'ui.log':
      console.info('[ui.log]', params.message)
      return null
    default:
      throw new Error(`mock: no answer for ${method}`)
  }
}
