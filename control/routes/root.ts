'use strict'

import { Route } from "./types.ts"

const route: Route = async (fastify, opts) => {
  fastify.get('/', async function (request, reply) {
    return 'this is an root'
  })
}

export default route

